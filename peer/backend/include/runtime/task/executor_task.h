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

    template <typename F>
    requires std::invocable<F> && std::move_constructible<F>
    ExecutorTask(F&& f, std::source_location loc = std::source_location::current())
        : location(loc) {
        if constexpr (sizeof(F) <= SOO_SIZE) {
            new (&buffer) F(std::forward<F>(f));
            vtable = &soo_vtable<F>;
        } else {
            auto ptr = std::make_unique<F>(std::forward<F>(f));
            new (&buffer) std::unique_ptr<F>(std::move(ptr));
            vtable = &heap_vtable<F>;
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
