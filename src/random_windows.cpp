#include "random.hpp"

#ifdef _WIN32

#include <windows.h>
#include <bcrypt.h>
#include <limits>
#include <stdexcept>

namespace mykeygen_internal {

void secure_random_bytes(
    void* destination,
    std::size_t count)
{
    auto* output =
        static_cast<unsigned char*>(destination);

    while (count > 0) {
        const std::size_t chunk =
            count >
                    static_cast<std::size_t>(
                        std::numeric_limits<ULONG>::max())
                ? static_cast<std::size_t>(
                      std::numeric_limits<ULONG>::max())
                : count;

        const NTSTATUS status =
            BCryptGenRandom(
                nullptr,
                output,
                static_cast<ULONG>(chunk),
                BCRYPT_USE_SYSTEM_PREFERRED_RNG);

        if (status != 0) {
            throw std::runtime_error(
                "failed to generate random data");
        }

        output += chunk;
        count -= chunk;
    }
}

} // namespace mykeygen_internal

#endif