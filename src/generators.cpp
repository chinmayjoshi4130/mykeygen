#include "generators.hpp"

#include "charset.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <limits>
#include <cctype>

static std::string generate_string(
    RandomSource& random,
    const std::string& charset,
    uint64_t length)
{
    if (charset.empty()) {
        throw std::runtime_error(
            "character set cannot be empty"
        );
    }

    if (length >
        static_cast<uint64_t>(
            std::numeric_limits<size_t>::max()
        )) {

        throw std::runtime_error(
            "length is too large"
        );
    }

    std::string result;

    result.reserve(
        static_cast<size_t>(length)
    );

    for (uint64_t i = 0;
         i < length;
         ++i) {

        const uint64_t index =
            random.range(
                0,
                charset.size() - 1
            );

        result.push_back(
            charset[
                static_cast<size_t>(index)
            ]
        );
    }

    return result;
}

static std::string trim(
    const std::string& value)
{
    size_t begin = 0;
    size_t end = value.size();

    while (
        begin < end &&
        std::isspace(
            static_cast<unsigned char>(
                value[begin]
            )
        )
    ) {
        ++begin;
    }

    while (
        end > begin &&
        std::isspace(
            static_cast<unsigned char>(
                value[end - 1]
            )
        )
    ) {
        --end;
    }

    return value.substr(
        begin,
        end - begin
    );
}

static std::vector<std::string> load_words(
    const std::string& path)
{
    std::ifstream file(path);

    if (!file) {
        throw std::runtime_error(
            "cannot open word file: " +
            path
        );
    }

    std::vector<std::string> words;

    std::string line;

    while (std::getline(file, line)) {

        line = trim(line);

        if (!line.empty()) {
            words.push_back(
                std::move(line)
            );
        }
    }

    if (words.empty()) {
        throw std::runtime_error(
            "word file contains no usable words"
        );
    }

    return words;
}


// ============================================================
// STRING
// ============================================================

void generate_string_mode(
    RandomSource& random,
    const Options& options,
    Output& output)
{
    const std::string charset =
        resolve_set(options.set);

    std::ostream& out =
        output.stream();

    if (options.separator.empty()) {

        for (uint64_t i = 0;
             i < options.count;
             ++i) {

            out
                << generate_string(
                    random,
                    charset,
                    options.length
                )
                << '\n';
        }

        return;
    }

    for (uint64_t i = 0;
         i < options.count;
         ++i) {

        if (i > 0) {
            out << options.separator;
        }

        out
            << generate_string(
                random,
                charset,
                options.length
            );
    }

    out << '\n';
}


// ============================================================
// RANGE
// ============================================================

void generate_range_mode(
    RandomSource& random,
    const Options& options,
    Output& output)
{
    std::ostream& out =
        output.stream();

    for (uint64_t i = 0;
         i < options.count;
         ++i) {

        out
            << random.signed_range(
                options.range_start,
                options.range_end
            )
            << '\n';
    }
}


// ============================================================
// FILE
// ============================================================

void generate_file_mode(
    RandomSource& random,
    const Options& options,
    Output& output)
{
    const auto words =
        load_words(options.word_file);

    std::ostream& out =
        output.stream();

    const uint64_t last =
        static_cast<uint64_t>(
            words.size() - 1
        );

    if (options.separator.empty()) {

        for (uint64_t i = 0;
             i < options.count;
             ++i) {

            const uint64_t index =
                random.range(
                    0,
                    last
                );

            out
                << words[
                    static_cast<size_t>(
                        index
                    )
                ]
                << '\n';
        }

        return;
    }

    for (uint64_t i = 0;
         i < options.count;
         ++i) {

        if (i > 0) {
            out << options.separator;
        }

        const uint64_t index =
            random.range(
                0,
                last
            );

        out
            << words[
                static_cast<size_t>(
                    index
                )
            ];
    }

    out << '\n';
}


// ============================================================
// BYTES
// ============================================================

static std::string encode_hex(
    const std::vector<uint8_t>& data)
{
    static constexpr char hex[] =
        "0123456789abcdef";

    std::string result;

    result.reserve(
        data.size() * 2
    );

    for (uint8_t byte : data) {

        result.push_back(
            hex[(byte >> 4) & 0x0f]
        );

        result.push_back(
            hex[byte & 0x0f]
        );
    }

    return result;
}

static std::string encode_base64(
    const std::vector<uint8_t>& data)
{
    static constexpr char alphabet[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string result;

    result.reserve(
        ((data.size() + 2) / 3) * 4
    );

    size_t i = 0;

    while (i + 2 < data.size()) {

        const uint32_t value =
            (static_cast<uint32_t>(data[i]) << 16) |
            (static_cast<uint32_t>(data[i + 1]) << 8) |
            static_cast<uint32_t>(data[i + 2]);

        result.push_back(
            alphabet[(value >> 18) & 0x3f]
        );

        result.push_back(
            alphabet[(value >> 12) & 0x3f]
        );

        result.push_back(
            alphabet[(value >> 6) & 0x3f]
        );

        result.push_back(
            alphabet[value & 0x3f]
        );

        i += 3;
    }

    const size_t remaining =
        data.size() - i;

    if (remaining == 1) {

        const uint32_t value =
            static_cast<uint32_t>(
                data[i]
            ) << 16;

        result.push_back(
            alphabet[(value >> 18) & 0x3f]
        );

        result.push_back(
            alphabet[(value >> 12) & 0x3f]
        );

        result.push_back('=');
        result.push_back('=');

    } else if (remaining == 2) {

        const uint32_t value =
            (static_cast<uint32_t>(data[i]) << 16) |
            (static_cast<uint32_t>(data[i + 1]) << 8);

        result.push_back(
            alphabet[(value >> 18) & 0x3f]
        );

        result.push_back(
            alphabet[(value >> 12) & 0x3f]
        );

        result.push_back(
            alphabet[(value >> 6) & 0x3f]
        );

        result.push_back('=');
    }

    return result;
}

void generate_bytes_mode(
    RandomSource& random,
    const Options& options,
    Output& output)
{
    std::ostream& out =
        output.stream();

    std::vector<uint8_t> data;

    data.resize(
        static_cast<size_t>(
            options.byte_count
        )
    );

    for (uint64_t i = 0;
         i < options.count;
         ++i) {

        random.bytes(
            data.data(),
            data.size()
        );

        std::string encoded;

        if (
            options.byte_encoding ==
            ByteEncoding::Hex
        ) {
            encoded =
                encode_hex(data);
        } else {
            encoded =
                encode_base64(data);
        }

        out << encoded << '\n';
    }
}