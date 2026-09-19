#pragma once

#include <cstddef>
#include <cstdint>

class RandomSource {
public:
    RandomSource();
    ~RandomSource();

    RandomSource(const RandomSource&) = delete;
    RandomSource& operator=(const RandomSource&) = delete;

    uint8_t byte();

    void bytes(void* buffer, size_t size);

    uint64_t uint64();

    uint64_t range(
        uint64_t min,
        uint64_t max
    );

    int64_t signed_range(
        int64_t min,
        int64_t max
    );

private:
    int fd_ = -1;

    void read_exact(
        void* buffer,
        size_t size
    );
};