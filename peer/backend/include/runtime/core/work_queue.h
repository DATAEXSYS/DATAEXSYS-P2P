#pragma once

#include <atomic>
#include <vector>
#include <optional>
#include <memory>

namespace runtime {

/**
 * @brief A lock-free Chase-Lev Deque for work-stealing.
 * 
 * Supports:
 * - push: (Owner only) LIFO
 * - pop:  (Owner only) LIFO
 * - steal: (Thieves) FIFO
 */
template <typename T>
class WorkStealingQueue {
public:
    WorkStealingQueue(size_t capacity = 1024)
        : bottom(0), top(0) {
        array.store(new Array(capacity), std::memory_order_relaxed);
    }

    ~WorkStealingQueue() {
        delete array.load(std::memory_order_relaxed);
    }

    // Owner: Push to bottom
    void push(T task) {
        long b = bottom.load(std::memory_order_relaxed);
        long t = top.load(std::memory_order_acquire);
        Array* a = array.load(std::memory_order_relaxed);

        if (b - t > static_cast<long>(a->capacity - 1)) {
            // Resize (simplified: just grow)
            a = a->grow(b, t);
            array.store(a, std::memory_order_release);
        }

        a->put(b, std::move(task));
        std::atomic_thread_fence(std::memory_order_release);
        bottom.store(b + 1, std::memory_order_relaxed);
    }

    // Owner: Pop from bottom
    std::optional<T> pop() {
        long b = bottom.load(std::memory_order_relaxed) - 1;
        Array* a = array.load(std::memory_order_relaxed);
        bottom.store(b, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        long t = top.load(std::memory_order_relaxed);

        if (t <= b) {
            T task = a->get(b);
            if (t == b) {
                if (!top.compare_exchange_strong(t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed)) {
                    bottom.store(b + 1, std::memory_order_relaxed);
                    return std::nullopt;
                }
                bottom.store(b + 1, std::memory_order_relaxed);
            }
            return std::move(task);
        } else {
            bottom.store(b + 1, std::memory_order_relaxed);
            return std::nullopt;
        }
    }

    // Thief: Steal from top
    std::optional<T> steal() {
        long t = top.load(std::memory_order_acquire);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        long b = bottom.load(std::memory_order_acquire);

        if (t < b) {
            Array* a = array.load(std::memory_order_consume);
            T task = a->get(t);
            if (!top.compare_exchange_strong(t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed)) {
                return std::nullopt;
            }
            return std::move(task);
        } else {
            return std::nullopt;
        }
    }

private:
    struct Array {
        size_t capacity;
        size_t mask;
        std::vector<T> buffer;

        Array(size_t cap) : capacity(cap), mask(cap - 1), buffer(cap) {}

        void put(long i, T task) {
            buffer[i & mask] = std::move(task);
        }

        T get(long i) {
            return std::move(buffer[i & mask]);
        }

        Array* grow(long b, long t) {
            Array* new_array = new Array(capacity * 2);
            for (long i = t; i < b; ++i) {
                new_array->put(i, get(i));
            }
            return new_array;
        }
    };

    std::atomic<long> bottom;
    std::atomic<long> top;
    std::atomic<Array*> array;
};

} // namespace runtime
