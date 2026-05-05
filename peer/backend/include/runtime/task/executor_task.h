#pragma once

#include <concepts>
#include <memory_resource>
#include <source_location>
#include <utility>

namespace runtime {

/**
 * @brief A move-only, type-erased task wrapper optimized for the runtime.
 * 
 * Uses Small Object Optimization (SOO) to store small callables inline.
 */
class ExecutorTask {
public:
    static constexpr size_t SOO_SIZE = 64 - sizeof(void*) - sizeof(std::source_location);

    ExecutorTask() : vtable(nullptr) {}

    explicit operator bool() const { return vtable != nullptr; }

    template <typename F>
    requires std::invocable<std::decay_t<F>> && std::move_constructible<std::decay_t<F>>
    ExecutorTask(F&& f, std::source_location loc = std::source_location::current())
        : location(loc) {
        using DecayedF = std::decay_t<F>;
        if constexpr (sizeof(DecayedF) <= SOO_SIZE) {
            new (&buffer) DecayedF(std::forward<F>(f));
            vtable = &soo_vtable<DecayedF>;
        } else {
            auto ptr = std::make_unique<DecayedF>(std::forward<F>(f));
            new (&buffer) std::unique_ptr<DecayedF>(std::move(ptr));
            vtable = &heap_vtable<DecayedF>;
        }
    }


    ~ExecutorTask() {
        if (vtable) vtable->destroy(&buffer);
    }

    ExecutorTask(const ExecutorTask&) = delete;
    ExecutorTask& operator=(const ExecutorTask&) = delete;

    ExecutorTask(ExecutorTask&& other) noexcept
        : vtable(std::exchange(other.vtable, nullptr)),
          location(other.location) {
        if (vtable) vtable->move(&other.buffer, &buffer);
    }

    ExecutorTask& operator=(ExecutorTask&& other) noexcept {
        if (this != &other) {
            if (vtable) vtable->destroy(&buffer);
            vtable = std::exchange(other.vtable, nullptr);
            location = other.location;
            if (vtable) vtable->move(&other.buffer, &buffer);
        }
        return *this;
    }

    void operator()() {
        if (vtable) vtable->invoke(&buffer);
    }

    const std::source_location& get_location() const { return location; }

private:
    struct VTable {
        void (*invoke)(void*);
        void (*destroy)(void*);
        void (*move)(void*, void*);
    };

    template <typename F>
    static constexpr VTable soo_vtable = {
        .invoke = [](void* buf) { (*static_cast<F*>(buf))(); },
        .destroy = [](void* buf) { static_cast<F*>(buf)->~F(); },
        .move = [](void* src, void* dst) { new (dst) F(std::move(*static_cast<F*>(src))); }
    };

    template <typename F>
    static constexpr VTable heap_vtable = {
        .invoke = [](void* buf) { (**static_cast<std::unique_ptr<F>*>(buf))(); },
        .destroy = [](void* buf) { static_cast<std::unique_ptr<F>*>(buf)->~unique_ptr(); },
        .move = [](void* src, void* dst) { new (dst) std::unique_ptr<F>(std::move(*static_cast<std::unique_ptr<F>*>(src))); }
    };

    alignas(std::max_align_t) char buffer[SOO_SIZE];
    const VTable* vtable = nullptr;
    std::source_location location;
};

} // namespace runtime
