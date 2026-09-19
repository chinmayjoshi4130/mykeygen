#pragma once

#include <cstddef>
#include <cstdint>

class RandomSource {
public:
    RandomSource();
    ~RandomSource();

    RandomSource(const RandomSource&) = delete;
    RandomSource& operator=(const RandomSource&) = delete;

    std::uint8_t byte();

    void bytes(
        void* destination,
        std::size_t count);

    std::uint64_t uint64();

    std::uint64_t range(
        std::uint64_t min,
        std::uint64_t max);

private:
    int fd_;
};