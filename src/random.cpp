#include "random.hpp"

#include <cerrno>
#include <cstdint>
#include <fcntl.h>
#include <limits>
#include <stdexcept>
#include <unistd.h>

RandomSource::RandomSource()
    : fd_(-1)
{
    fd_ = ::open("/dev/urandom", O_RDONLY);

    if (fd_ < 0) {
        throw std::runtime_error(
            "failed to open /dev/urandom");
    }
}

RandomSource::~RandomSource()
{
    if (fd_ >= 0) {
        ::close(fd_);
    }
}

std::uint8_t RandomSource::byte()
{
    std::uint8_t value = 0;
    bytes(&value, sizeof(value));
    return value;
}

void RandomSource::bytes(
    void* destination,
    std::size_t count)
{
    auto* output =
        static_cast<std::uint8_t*>(destination);

    std::size_t offset = 0;

    while (offset < count) {
        const ssize_t result =
            ::read(
                fd_,
                output + offset,
                count - offset);

        if (result > 0) {
            offset += static_cast<std::size_t>(result);
            continue;
        }

        if (result < 0 && errno == EINTR) {
            continue;
        }

        throw std::runtime_error(
            "failed to read random data");
    }
}

std::uint64_t RandomSource::uint64()
{
    std::uint64_t value = 0;
    bytes(&value, sizeof(value));
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

    const std::uint64_t span = max - min + 1;

    /*
     * Rejection sampling avoids modulo bias.
     */
    const std::uint64_t limit =
        std::numeric_limits<std::uint64_t>::max()
        - (
            std::numeric_limits<std::uint64_t>::max()
            % span
        );

    while (true) {
        const std::uint64_t value = uint64();

        if (value < limit) {
            return min + (value % span);
        }
    }
}