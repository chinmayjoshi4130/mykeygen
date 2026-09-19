#include "mykeygen/mykeygen.hpp"

#include "random.hpp"

#include <stdexcept>
#include <string>

namespace mykeygen {

namespace {

RandomSource& random_source()
{
    static RandomSource source;
    return source;
}

std::string generate_from_charset(
    const std::string& charset,
    std::size_t length)
{
    if (charset.empty()) {
        throw std::invalid_argument("character set cannot be empty");
    }

    std::string result;
    result.reserve(length);

    for (std::size_t i = 0; i < length; ++i) {
        const std::uint64_t index =
            random_source().range(0, charset.size() - 1);

        result.push_back(charset[static_cast<std::size_t>(index)]);
    }

    return result;
}

} // namespace

Bytes bytes(std::size_t count)
{
    Bytes result(count);

    if (count != 0) {
        random_source().bytes(result.data(), result.size());
    }

    return result;
}

std::string hex(std::size_t count)
{
    static constexpr char digits[] = "0123456789abcdef";

    const Bytes data = bytes(count);

    std::string result;
    result.reserve(count * 2);

    for (const Byte byte : data) {
        result.push_back(digits[(byte >> 4) & 0x0f]);
        result.push_back(digits[byte & 0x0f]);
    }

    return result;
}

std::string base64(std::size_t count)
{
    static constexpr char alphabet[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    const Bytes data = bytes(count);

    std::string result;
    result.reserve(((count + 2) / 3) * 4);

    std::size_t i = 0;

    while (i + 3 <= data.size()) {
        const std::uint32_t value =
            (static_cast<std::uint32_t>(data[i]) << 16) |
            (static_cast<std::uint32_t>(data[i + 1]) << 8) |
            static_cast<std::uint32_t>(data[i + 2]);

        result.push_back(alphabet[(value >> 18) & 0x3f]);
        result.push_back(alphabet[(value >> 12) & 0x3f]);
        result.push_back(alphabet[(value >> 6) & 0x3f]);
        result.push_back(alphabet[value & 0x3f]);

        i += 3;
    }

    const std::size_t remaining = data.size() - i;

    if (remaining == 1) {
        const std::uint32_t value =
            static_cast<std::uint32_t>(data[i]) << 16;

        result.push_back(alphabet[(value >> 18) & 0x3f]);
        result.push_back(alphabet[(value >> 12) & 0x3f]);
        result.push_back('=');
        result.push_back('=');
    } else if (remaining == 2) {
        const std::uint32_t value =
            (static_cast<std::uint32_t>(data[i]) << 16) |
            (static_cast<std::uint32_t>(data[i + 1]) << 8);

        result.push_back(alphabet[(value >> 18) & 0x3f]);
        result.push_back(alphabet[(value >> 12) & 0x3f]);
        result.push_back(alphabet[(value >> 6) & 0x3f]);
        result.push_back('=');
    }

    return result;
}

std::uint64_t range(
    std::uint64_t min,
    std::uint64_t max)
{
    return random_source().range(min, max);
}

std::string string(
    const std::string& charset,
    std::size_t length)
{
    return generate_from_charset(charset, length);
}

std::string alnum(std::size_t length)
{
    return generate_from_charset(
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789",
        length);
}

std::string numeric(std::size_t length)
{
    return generate_from_charset(
        "0123456789",
        length);
}

std::string upper(std::size_t length)
{
    return generate_from_charset(
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        length);
}

std::string lower(std::size_t length)
{
    return generate_from_charset(
        "abcdefghijklmnopqrstuvwxyz",
        length);
}

std::string hexadecimal(std::size_t length)
{
    return generate_from_charset(
        "0123456789abcdef",
        length);
}

std::string base64_string(std::size_t length)
{
    return generate_from_charset(
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/",
        length);
}

} // namespace mykeygen