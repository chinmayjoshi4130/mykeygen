#include "random.hpp"

#include <cerrno>
#include <fcntl.h>
#include <limits>
#include <stdexcept>
#include <unistd.h>

RandomSource::RandomSource()
{
    fd_ = open("/dev/urandom", O_RDONLY);

    if (fd_ < 0) {
        throw std::runtime_error(
            "cannot open /dev/urandom"
        );
    }
}

RandomSource::~RandomSource()
{
    if (fd_ >= 0) {
        close(fd_);
    }
}

uint8_t RandomSource::byte()
{
    uint8_t value;

    read_exact(
        &value,
        sizeof(value)
    );

    return value;
}

void RandomSource::bytes(
    void* buffer,
    size_t size)
{
    if (size == 0) {
        return;
    }

    read_exact(
        buffer,
        size
    );
}

uint64_t RandomSource::uint64()
{
    uint64_t value;

    read_exact(
        &value,
        sizeof(value)
    );

    return value;
}

uint64_t RandomSource::range(
    uint64_t min,
    uint64_t max)
{
    if (min > max) {
        throw std::runtime_error(
            "invalid random range"
        );
    }

    /*
     * Number of values in the range.
     *
     * If max-min+1 overflows, the requested
     * range contains all uint64_t values.
     */
    const uint64_t size =
        max - min + 1;

    if (size == 0) {
        return uint64();
    }

    /*
     * Rejection sampling.
     *
     * Accept only a complete number of
     * equally-sized buckets.
     */
    const uint64_t limit =
        std::numeric_limits<uint64_t>::max()
        -
        (
            std::numeric_limits<uint64_t>::max()
            % size
        );

    while (true) {
        const uint64_t value = uint64();

        if (value < limit) {
            return min + (value % size);
        }
    }
}

int64_t RandomSource::signed_range(
    int64_t min,
    int64_t max)
{
    if (min > max) {
        throw std::runtime_error(
            "invalid random range"
        );
    }

    /*
     * Convert the signed interval into an
     * unsigned offset without overflowing.
     *
     * The full int64_t range contains 2^64
     * values, which is represented by size == 0.
     */

    uint64_t size;

    if (min < 0 && max >= 0) {

        const uint64_t negative =
            static_cast<uint64_t>(
                -(min + 1)
            ) + 1;

        const uint64_t positive =
            static_cast<uint64_t>(max);

        size =
            negative +
            positive +
            1;

    } else if (min >= 0) {

        size =
            static_cast<uint64_t>(max - min)
            + 1;

    } else {

        /*
         * Both values are negative.
         *
         * -(max-min) can be calculated safely
         * through unsigned arithmetic.
         */
        const uint64_t distance =
            static_cast<uint64_t>(
                max
            ) -
            static_cast<uint64_t>(
                min
            );

        size = distance + 1;
    }

    /*
     * Full int64_t range.
     */
    if (size == 0) {
        const uint64_t offset = uint64();

        /*
         * Conversion from uint64_t to int64_t
         * gives the desired two's-complement
         * mapping on the supported platforms.
         */
        return static_cast<int64_t>(offset);
    }

    const uint64_t offset =
        range(0, size - 1);

    /*
     * Addition is safe because offset is
     * guaranteed to remain inside [min,max].
     */
    return min +
        static_cast<int64_t>(offset);
}

void RandomSource::read_exact(
    void* buffer,
    size_t size)
{
    auto* ptr =
        static_cast<unsigned char*>(buffer);

    while (size > 0) {

        const ssize_t result =
            read(
                fd_,
                ptr,
                size
            );

        if (result < 0) {

            if (errno == EINTR) {
                continue;
            }

            throw std::runtime_error(
                "failed to read /dev/urandom"
            );
        }

        if (result == 0) {
            throw std::runtime_error(
                "unexpected end of /dev/urandom"
            );
        }

        ptr += result;
        size -= static_cast<size_t>(result);
    }
}