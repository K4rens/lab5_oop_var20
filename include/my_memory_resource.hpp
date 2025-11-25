#pragma once

#include <vector>
#include <algorithm>
#include <stdexcept>
#include <memory_resource>


class CustomMemoryPool : public std::pmr::memory_resource { 
private:
    struct MemoryChunk {
        size_t chunk_size;
        size_t start_pos;
        MemoryChunk(size_t pos = 0, size_t sz = 0) : start_pos(pos), chunk_size(sz) {}
    };

    static constexpr size_t POOL_CAPACITY{1024};
    char storage[POOL_CAPACITY];
    std::vector<MemoryChunk> active_chunks;

public:
    CustomMemoryPool() = default;

protected:
    void *do_allocate(size_t required_bytes, size_t alignment) override {
        size_t current_pos = 0;

        std::sort(active_chunks.begin(), active_chunks.end(), 
                 [](const MemoryChunk &a, const MemoryChunk &b) { return a.start_pos < b.start_pos; });

        for (const MemoryChunk &chunk : active_chunks) {
            size_t aligned_pos = (current_pos + alignment - 1) & ~(alignment - 1);
            if (aligned_pos + required_bytes <= chunk.start_pos) {
                break;
            }
            current_pos = chunk.start_pos + chunk.chunk_size;
        }

        size_t final_pos = (current_pos + alignment - 1) & ~(alignment - 1);
        if (final_pos + required_bytes > POOL_CAPACITY) {
            throw std::bad_alloc();
        }

        active_chunks.emplace_back(final_pos, required_bytes);
        return storage + final_pos;
    }

    void do_deallocate(void *ptr, size_t bytes, size_t alignment) override {
        char *target = static_cast<char *>(ptr);

        for (auto it = active_chunks.begin(); it != active_chunks.end(); ++it) {
            if (target == storage + it->start_pos) {
                active_chunks.erase(it);
                return;
            }
        }

        throw std::logic_error("Invalid deallocation attempt");
    }

    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
        return this == &other;
    }
};
