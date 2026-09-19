#include "random.hpp"

#include <stdexcept>

std::uint8_t RandomSource::byte()
{
    std::uint8_t value = 0;

    bytes(
        &value,
        sizeof(value));

    return value;
}

void RandomSource::bytes(
    void* destination,
    std::size_t count)
{
    if (count == 0) {
        return;
    }

    mykeygen_internal::secure_random_bytes(
        destination,
        count);
}

std::uint64_t RandomSource::uint64()
{
    std::uint64_t value = 0;

    bytes(
        &value,
        sizeof(value));

    return value;
}

std::uint64_t RandomSource::range(
    std::uint64_t min,
    std::uint64_t max)
{
    if (min > max) {
        throw std::invalid_argument(
            "invalid random range");
    }

    if (min == max) {
        return min;
    }

    const std::uint64_t span =
        max - min + 1;

    const std::uint64_t threshold =
        -span % span;

    while (true) {
        const std::uint64_t value = uint64();

        if (value >= threshold) {
            return min + (value % span);
        }
    }
}