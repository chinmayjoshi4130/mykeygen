#pragma once

#include "output.hpp"

#include <cstddef>
#include <cstdint>
#include <string>

enum class Mode {
    Character,
    Range,
    File,
    Bytes
};

enum class ByteEncoding {
    Hex,
    Base64
};

struct Options {
    Mode mode = Mode::Character;

    std::string charset = "alnum";
    std::size_t length = 32;
    std::size_t count = 1;

    std::uint64_t range_min = 0;
    std::uint64_t range_max = 0;

    std::string file;

    std::string separator;

    std::size_t byte_count = 0;
    ByteEncoding byte_encoding = ByteEncoding::Hex;

    OutputMode output_mode = OutputMode::Stdout;
    std::string output_file;

    bool help = false;
    bool api = false;
};

Options parse_options(
    int argc,
    char** argv);

void validate_options(
    const Options& options);

void print_help();

void print_api();