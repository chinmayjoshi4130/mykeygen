#include "options.hpp"

#include "charset.hpp"

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

[[noreturn]]
static void error(
    const std::string& message)
{
    throw std::runtime_error(message);
}

static bool is_unsigned_integer(
    const std::string& value)
{
    if (value.empty()) {
        return false;
    }

    for (char c : value) {

        if (!std::isdigit(
                static_cast<unsigned char>(c)
            )) {
            return false;
        }
    }

    return true;
}

static uint64_t parse_positive(
    const std::string& value,
    const std::string& name)
{
    if (!is_unsigned_integer(value)) {
        error(
            name +
            " must be a positive number"
        );
    }

    try {

        const unsigned long long result =
            std::stoull(value);

        if (result == 0) {
            error(
                name +
                " must be greater than zero"
            );
        }

        return static_cast<uint64_t>(
            result
        );
    }
    catch (...) {
        error(
            name +
            " is too large"
        );
    }
}

static int64_t parse_integer(
    const std::string& value,
    const std::string& name)
{
    if (value.empty()) {
        error(
            name +
            " must be an integer"
        );
    }

    size_t start = 0;

    if (
        value[0] == '-' ||
        value[0] == '+'
    ) {
        start = 1;
    }

    if (start == value.size()) {
        error(
            name +
            " must be an integer"
        );
    }

    for (size_t i = start;
         i < value.size();
         ++i) {

        if (!std::isdigit(
                static_cast<unsigned char>(
                    value[i]
                )
            )) {

            error(
                name +
                " must be an integer"
            );
        }
    }

    try {
        return std::stoll(value);
    }
    catch (...) {
        error(
            name +
            " is out of range"
        );
    }
}

void usage()
{
    std::cout << R"(mykeygen - random password/key generator

Usage:
  mykeygen [options]

CHARACTER GENERATOR
  -s, --set <set>             Character set:
                                alnum
                                num
                                upper
                                lower
                                hex
                                base64
                                <custom>

  -l, --length <length>       Length of each generated value

  -c, --count <count>         Number of outputs

  --sep <separator>           Build multiple values into one line

NUMBER GENERATOR
  --range <start> <end>       Random number in inclusive range

  -c, --count <count>         Number of random numbers

FILE WORD GENERATOR
  -f, --file <path>           Word list file

  -c, --count <count>         Number of words

  --sep <separator>           Separator between words

BYTE GENERATOR
  --bytes <count>             Generate random bytes

  --hex                       Encode bytes as hexadecimal

  --base64                    Encode bytes as Base64

  -c, --count <count>         Number of byte outputs

OUTPUT
  -o <file>                   Create/overwrite file

  -oa <file>                  Create/append to file

UTILITY
  api                         Show command/API tree

  help
  -h, --help                  Show this help

CUSTOM CHARACTER SETS
  Custom sets support ranges:

    A-Z
    a-z
    0-9

  Ranges can be combined with literal characters:

    A-Za-z0-9
    A-F0-9
    A-Za-z0-9!@#$%
    abc123

EXAMPLES
  mykeygen

  mykeygen -s alnum -l 32

  mykeygen -s hex -l 64 -c 3

  mykeygen -s 'A-Za-z0-9!@#$%' -l 32

  mykeygen -s 'A-F0-9' -l 64

  mykeygen -s 'abc123' -l 20

  mykeygen -s alnum -l 8 -c 4 --sep -

  mykeygen -s hex -l 4 -c 8 --sep :

  mykeygen --range 1 100

  mykeygen --range 1000 9999 -c 10

  mykeygen -f words.txt -c 4 --sep -

  mykeygen -f words.txt -c 6 --sep _

  mykeygen -s hex -l 64 -c 10 -o keys.txt

  mykeygen -f words.txt -c 10 --sep - -oa passwords.txt

  mykeygen --bytes 32 --hex

  mykeygen --bytes 32 --base64

  mykeygen --bytes 32 --hex -c 10
)";
}

void api()
{
    std::cout << R"(mykeygen
│
├── generator
│   │
│   ├── character
│   │   ├── -s, --set <set>
│   │   │   ├── alnum
│   │   │   ├── num
│   │   │   ├── upper
│   │   │   ├── lower
│   │   │   ├── hex
│   │   │   ├── base64
│   │   │   └── <custom>
│   │   │       ├── literal
│   │   │       └── ranges
│   │   │
│   │   ├── -l, --length <length>
│   │   ├── -c, --count <count>
│   │   └── --sep <separator>
│   │
│   ├── number
│   │   ├── --range <start> <end>
│   │   └── -c, --count <count>
│   │
│   ├── file
│   │   ├── -f, --file <path>
│   │   ├── -c, --count <count>
│   │   └── --sep <separator>
│   │
│   └── bytes
│       ├── --bytes <count>
│       ├── --hex
│       ├── --base64
│       └── -c, --count <count>
│
├── output
│   ├── stdout
│   │   └── default
│   ├── -o <file>
│   │   └── overwrite/create
│   └── -oa <file>
│       └── append/create
│
├── utility
│   ├── api
│   └── help
│
└── custom-set
    ├── A-Z
    ├── a-z
    ├── 0-9
    ├── literal characters
    └── mixed ranges + literals
)";
}

Options parse_arguments(
    int argc,
    char* argv[])
{
    Options options;

    for (int i = 1;
         i < argc;
         ++i) {

        const std::string arg =
            argv[i];

        if (
            arg == "-h" ||
            arg == "--help" ||
            arg == "help"
        ) {
            usage();
            std::exit(0);
        }

        if (arg == "api") {
            api();
            std::exit(0);
        }

        if (
            arg == "-s" ||
            arg == "--set"
        ) {

            if (
                options.mode !=
                Mode::String
            ) {
                error(
                    "--set cannot be combined "
                    "with another generator"
                );
            }

            if (++i >= argc) {
                error(
                    arg +
                    " requires a character set"
                );
            }

            options.set =
                argv[i];

            continue;
        }

        if (
            arg == "-l" ||
            arg == "--length"
        ) {

            if (
                options.mode !=
                Mode::String
            ) {
                error(
                    "--length cannot be used "
                    "with another generator"
                );
            }

            if (++i >= argc) {
                error(
                    arg +
                    " requires a number"
                );
            }

            options.length =
                parse_positive(
                    argv[i],
                    "length"
                );

            continue;
        }

        if (
            arg == "-c" ||
            arg == "--count"
        ) {

            if (++i >= argc) {
                error(
                    arg +
                    " requires a number"
                );
            }

            options.count =
                parse_positive(
                    argv[i],
                    "count"
                );

            continue;
        }

        if (arg == "--sep") {

            if (++i >= argc) {
                error(
                    "--sep requires a separator"
                );
            }

            options.separator =
                argv[i];

            continue;
        }

        if (arg == "--range") {

            if (
                options.mode !=
                Mode::String
            ) {
                error(
                    "--range cannot be combined "
                    "with another generator"
                );
            }

            if (i + 2 >= argc) {
                error(
                    "--range requires "
                    "<start> <end>"
                );
            }

            options.range_start =
                parse_integer(
                    argv[++i],
                    "range start"
                );

            options.range_end =
                parse_integer(
                    argv[++i],
                    "range end"
                );

            options.mode =
                Mode::Range;

            continue;
        }

        if (
            arg == "-f" ||
            arg == "--file"
        ) {

            if (
                options.mode !=
                Mode::String
            ) {
                error(
                    "--file cannot be combined "
                    "with another generator"
                );
            }

            if (++i >= argc) {
                error(
                    arg +
                    " requires a file path"
                );
            }

            options.word_file =
                argv[i];

            options.mode =
                Mode::File;

            continue;
        }

        if (arg == "--bytes") {

            if (
                options.mode !=
                Mode::String
            ) {
                error(
                    "--bytes cannot be combined "
                    "with another generator"
                );
            }

            if (++i >= argc) {
                error(
                    "--bytes requires a number"
                );
            }

            options.byte_count =
                parse_positive(
                    argv[i],
                    "bytes"
                );

            options.mode =
                Mode::Bytes;

            continue;
        }

        if (arg == "--hex") {

            if (
                options.mode !=
                Mode::Bytes
            ) {
                error(
                    "--hex requires --bytes"
                );
            }

            options.byte_encoding =
                ByteEncoding::Hex;

            continue;
        }

        if (arg == "--base64") {

            if (
                options.mode !=
                Mode::Bytes
            ) {
                error(
                    "--base64 requires --bytes"
                );
            }

            options.byte_encoding =
                ByteEncoding::Base64;

            continue;
        }

        if (arg == "-o") {

            if (++i >= argc) {
                error(
                    "-o requires a file path"
                );
            }

            options.output_file =
                argv[i];

            options.output_mode =
                OutputMode::Overwrite;

            continue;
        }

        if (arg == "-oa") {

            if (++i >= argc) {
                error(
                    "-oa requires a file path"
                );
            }

            options.output_file =
                argv[i];

            options.output_mode =
                OutputMode::Append;

            continue;
        }

        error(
            "unknown option: " + arg
        );
    }

    return options;
}

void validate(
    const Options& options)
{
    if (
        options.mode ==
        Mode::Range
    ) {

        if (!options.separator.empty()) {
            error(
                "--sep cannot be used "
                "with --range"
            );
        }

        if (
            options.range_start >
            options.range_end
        ) {
            error(
                "range start must be less "
                "than or equal to end"
            );
        }

        return;
    }

    if (
        options.mode ==
        Mode::File
    ) {

        if (options.length != 16) {
            error(
                "--length cannot be used "
                "with --file"
            );
        }

        if (options.set != "alnum") {
            error(
                "--set cannot be used "
                "with --file"
            );
        }

        return;
    }

    if (
        options.mode ==
        Mode::Bytes
    ) {

        if (
            options.byte_count >
            static_cast<uint64_t>(
                std::numeric_limits<size_t>::max()
            )
        ) {
            error(
                "byte count is too large"
            );
        }

        if (!options.separator.empty()) {
            error(
                "--sep cannot be used "
                "with --bytes"
            );
        }

        return;
    }

    resolve_set(options.set);
}