#include "random.hpp"

#ifndef _WIN32

#include <cerrno>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>

namespace mykeygen_internal {

void secure_random_bytes(
    void* destination,
    std::size_t count)
{
    const int fd =
        ::open("/dev/urandom", O_RDONLY);

    if (fd < 0) {
        throw std::runtime_error(
            "failed to open /dev/urandom");
    }

    auto* output =
        static_cast<std::uint8_t*>(destination);

    std::size_t offset = 0;

    while (offset < count) {
        const ssize_t result =
            ::read(
                fd,
                output + offset,
                count - offset);

        if (result > 0) {
            offset += static_cast<std::size_t>(result);
            continue;
        }

        if (result < 0 && errno == EINTR) {
            continue;
        }

        ::close(fd);

        throw std::runtime_error(
            "failed to read random data");
    }

    ::close(fd);
}

} // namespace mykeygen_internal

#endif