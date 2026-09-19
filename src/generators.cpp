#include "generators.hpp"

#include <fstream>
#include <limits>
#include <stdexcept>

std::string generate_string(
    RandomSource& random,
    const std::string& charset,
    std::size_t length)
{
    if (charset.empty()) {
        throw std::invalid_argument(
            "character set cannot be empty");
    }

    std::string result;
    result.reserve(length);

    for (std::size_t i = 0; i < length; ++i) {
        const auto index =
            random.range(0, charset.size() - 1);

        result.push_back(
            charset[static_cast<std::size_t>(index)]);
    }

    return result;
}

std::vector<std::uint64_t> generate_numbers(
    RandomSource& random,
    std::uint64_t min,
    std::uint64_t max,
    std::size_t count)
{
    std::vector<std::uint64_t> result;
    result.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        result.push_back(random.range(min, max));
    }

    return result;
}

std::vector<std::string> generate_words(
    const std::string& path,
    std::size_t count)
{
    std::ifstream file(path);

    if (!file) {
        throw std::runtime_error(
            "failed to open word list: " + path);
    }

    std::vector<std::string> words;
    std::string word;

    while (std::getline(file, word)) {
        if (!word.empty()) {
            words.push_back(word);
        }
    }

    if (words.empty()) {
        throw std::runtime_error(
            "word list is empty");
    }

    RandomSource random;

    std::vector<std::string> result;
    result.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        const auto index =
            random.range(0, words.size() - 1);

        result.push_back(
            words[static_cast<std::size_t>(index)]);
    }

    return result;
}

std::vector<std::uint8_t> generate_bytes(
    RandomSource& random,
    std::size_t count)
{
    std::vector<std::uint8_t> result(count);

    if (!result.empty()) {
        random.bytes(
            result.data(),
            result.size());
    }

    return result;
}

std::string encode_hex(
    const std::vector<std::uint8_t>& data)
{
    static constexpr char digits[] =
        "0123456789abcdef";

    std::string result;
    result.reserve(data.size() * 2);

    for (const auto byte : data) {
        result.push_back(
            digits[(byte >> 4) & 0x0f]);

        result.push_back(
            digits[byte & 0x0f]);
    }

    return result;
}

std::string encode_base64(
    const std::vector<std::uint8_t>& data)
{
    static constexpr char alphabet[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string result;
    result.reserve(((data.size() + 2) / 3) * 4);

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

    const std::size_t remaining =
        data.size() - i;

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