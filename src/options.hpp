#pragma once

#include <cstdint>
#include <string>

enum class Mode {
    String,
    Range,
    File,
    Bytes
};

enum class OutputMode {
    Stdout,
    Overwrite,
    Append
};

enum class ByteEncoding {
    Hex,
    Base64
};

struct Options {
    Mode mode = Mode::String;

    std::string set = "alnum";

    uint64_t length = 16;
    uint64_t count = 1;

    std::string separator;

    int64_t range_start = 0;
    int64_t range_end = 0;

    std::string word_file;

    uint64_t byte_count = 0;
    ByteEncoding byte_encoding =
        ByteEncoding::Hex;

    std::string output_file;
    OutputMode output_mode =
        OutputMode::Stdout;
};

Options parse_arguments(
    int argc,
    char* argv[]
);

void validate(
    const Options& options
);

void usage();

void api();