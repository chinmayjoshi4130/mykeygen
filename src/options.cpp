#include "options.hpp"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

namespace {

std::size_t parse_size(
    const std::string& value,
    const char* name)
{
    if (value.empty() || value[0] == '-') {
        throw std::invalid_argument(
            std::string("invalid ") + name);
    }

    std::size_t position = 0;

    const unsigned long long parsed =
        std::stoull(value, &position);

    if (position != value.size()) {
        throw std::invalid_argument(
            std::string("invalid ") + name);
    }

    if (
        parsed >
        static_cast<unsigned long long>(
            std::numeric_limits<std::size_t>::max())
    ) {
        throw std::invalid_argument(
            std::string(name) + " is too large");
    }

    return static_cast<std::size_t>(parsed);
}

std::uint64_t parse_uint64(
    const std::string& value,
    const char* name)
{
    if (value.empty() || value[0] == '-') {
        throw std::invalid_argument(
            std::string("invalid ") + name);
    }

    std::size_t position = 0;

    const unsigned long long parsed =
        std::stoull(value, &position);

    if (position != value.size()) {
        throw std::invalid_argument(
            std::string("invalid ") + name);
    }

    return static_cast<std::uint64_t>(parsed);
}

bool is_option(const char* value)
{
    return value != nullptr && value[0] == '-';
}

void require_argument(
    int index,
    int argc,
    const char* option)
{
    if (index + 1 >= argc) {
        throw std::invalid_argument(
            std::string("missing argument for ") + option);
    }
}

} // namespace

Options parse_options(
    int argc,
    char** argv)
{
    Options options;

    if (argc == 1) {
        return options;
    }

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "help" || arg == "-h" || arg == "--help") {
            options.help = true;
            continue;
        }

        if (arg == "api") {
            options.api = true;
            continue;
        }

        if (arg == "-s" || arg == "--set") {
            require_argument(i, argc, argv[i]);
            options.mode = Mode::Character;
            options.charset = argv[++i];
            continue;
        }

        if (arg == "-l" || arg == "--length") {
            require_argument(i, argc, argv[i]);
            options.length =
                parse_size(argv[++i], "length");
            continue;
        }

        if (arg == "-c" || arg == "--count") {
            require_argument(i, argc, argv[i]);
            options.count =
                parse_size(argv[++i], "count");
            continue;
        }

        if (arg == "--sep") {
            require_argument(i, argc, argv[i]);
            options.separator = argv[++i];
            continue;
        }

        if (arg == "--range") {
            if (i + 2 >= argc) {
                throw std::invalid_argument(
                    "--range requires <start> <end>");
            }

            options.mode = Mode::Range;

            options.range_min =
                parse_uint64(argv[++i], "range start");

            options.range_max =
                parse_uint64(argv[++i], "range end");

            continue;
        }

        if (arg == "-f" || arg == "--file") {
            require_argument(i, argc, argv[i]);

            options.mode = Mode::File;
            options.file = argv[++i];

            continue;
        }

        if (arg == "--bytes") {
            require_argument(i, argc, argv[i]);

            options.mode = Mode::Bytes;
            options.byte_count =
                parse_size(argv[++i], "byte count");

            continue;
        }

        if (arg == "--hex") {
            if (options.mode == Mode::Bytes) {
                options.byte_encoding =
                    ByteEncoding::Hex;
            } else {
                options.mode = Mode::Character;
                options.charset = "hex";
            }

            continue;
        }

        if (arg == "--base64") {
            if (options.mode == Mode::Bytes) {
                options.byte_encoding =
                    ByteEncoding::Base64;
            } else {
                options.mode = Mode::Character;
                options.charset = "base64";
            }

            continue;
        }

        if (arg == "-o") {
            require_argument(i, argc, argv[i]);

            options.output_mode =
                OutputMode::Overwrite;

            options.output_file = argv[++i];

            continue;
        }

        if (arg == "-oa") {
            require_argument(i, argc, argv[i]);

            options.output_mode =
                OutputMode::Append;

            options.output_file = argv[++i];

            continue;
        }

        if (is_option(argv[i])) {
            throw std::invalid_argument(
                "unknown option: " + arg);
        }

        throw std::invalid_argument(
            "unknown argument: " + arg);
    }

    return options;
}

void validate_options(
    const Options& options)
{
    if (options.help || options.api) {
        return;
    }

    if (options.count == 0) {
        throw std::invalid_argument(
            "count must be greater than zero");
    }

    if (options.output_mode != OutputMode::Stdout &&
        options.output_file.empty()) {
        throw std::invalid_argument(
            "output file is required");
    }

    switch (options.mode) {
    case Mode::Character:
        if (options.length == 0) {
            throw std::invalid_argument(
                "length must be greater than zero");
        }
        break;

    case Mode::Range:
        if (options.range_min > options.range_max) {
            throw std::invalid_argument(
                "range start must not exceed range end");
        }
        break;

    case Mode::File:
        if (options.file.empty()) {
            throw std::invalid_argument(
                "word list file is required");
        }
        break;

    case Mode::Bytes:
        if (options.byte_count == 0) {
            throw std::invalid_argument(
                "byte count must be greater than zero");
        }

        if (!options.separator.empty()) {
            throw std::invalid_argument(
                "--sep cannot be used with --bytes");
        }

        break;
    }
}

void print_help()
{
    std::cout
        << "mykeygen - secure random value generator\n"
        << "\n"
        << "Usage:\n"
        << "  mykeygen [options]\n"
        << "\n"

        << "CHARACTER GENERATOR\n"
        << "  -s, --set <set>             Character set:\n"
        << "                                alnum\n"
        << "                                num\n"
        << "                                upper\n"
        << "                                lower\n"
        << "                                hex\n"
        << "                                base64\n"
        << "                                <custom>\n"
        << "\n"
        << "  -l, --length <length>       Length of each generated value\n"
        << "\n"
        << "  -c, --count <count>         Number of outputs\n"
        << "\n"
        << "  --sep <separator>           Build multiple values into one line\n"
        << "\n"

        << "NUMBER GENERATOR\n"
        << "  --range <start> <end>       Random number in inclusive range\n"
        << "\n"
        << "  -c, --count <count>         Number of random numbers\n"
        << "\n"

        << "FILE WORD GENERATOR\n"
        << "  -f, --file <path>           Word list file\n"
        << "\n"
        << "  -c, --count <count>         Number of words\n"
        << "\n"
        << "  --sep <separator>           Separator between words\n"
        << "\n"

        << "BYTE GENERATOR\n"
        << "  --bytes <count>             Generate random bytes\n"
        << "\n"
        << "  --hex                       Encode bytes as hexadecimal\n"
        << "\n"
        << "  --base64                    Encode bytes as Base64\n"
        << "\n"
        << "  -c, --count <count>         Number of byte outputs\n"
        << "\n"

        << "OUTPUT\n"
        << "  -o <file>                   Create/overwrite file\n"
        << "\n"
        << "  -oa <file>                  Create/append to file\n"
        << "\n"

        << "UTILITY\n"
        << "  api                         Show command/API tree\n"
        << "\n"
        << "  help\n"
        << "  -h, --help                  Show this help\n"
        << "\n"

        << "CUSTOM CHARACTER SETS\n"
        << "  Custom sets support ranges:\n"
        << "\n"
        << "    A-Z\n"
        << "    a-z\n"
        << "    0-9\n"
        << "\n"
        << "  Ranges can be combined with literal characters:\n"
        << "\n"
        << "    A-Za-z0-9\n"
        << "    A-F0-9\n"
        << "    A-Za-z0-9!@#$%\n"
        << "    abc123\n"
        << "\n"

        << "EXAMPLES\n"
        << "  mykeygen\n"
        << "\n"
        << "  mykeygen -s alnum -l 32\n"
        << "\n"
        << "  mykeygen -s hex -l 64 -c 3\n"
        << "\n"
        << "  mykeygen -s 'A-Za-z0-9!@#$%' -l 32\n"
        << "\n"
        << "  mykeygen -s 'A-F0-9' -l 64\n"
        << "\n"
        << "  mykeygen -s 'abc123' -l 20\n"
        << "\n"
        << "  mykeygen -s alnum -l 8 -c 4 --sep -\n"
        << "\n"
        << "  mykeygen -s hex -l 4 -c 8 --sep :\n"
        << "\n"
        << "  mykeygen --range 1 100\n"
        << "\n"
        << "  mykeygen --range 1000 9999 -c 10\n"
        << "\n"
        << "  mykeygen -f words.txt -c 4 --sep -\n"
        << "\n"
        << "  mykeygen -f words.txt -c 6 --sep _\n"
        << "\n"
        << "  mykeygen -s hex -l 64 -c 10 -o keys.txt\n"
        << "\n"
        << "  mykeygen -f words.txt -c 10 --sep - -oa passwords.txt\n"
        << "\n"
        << "  mykeygen --bytes 32 --hex\n"
        << "\n"
        << "  mykeygen --bytes 32 --base64\n"
        << "\n"
        << "  mykeygen --bytes 32 --hex -c 10\n";
}

void print_api()
{
    std::cout
        << "mykeygen\n"
        << "│\n"
        << "├── generator\n"
        << "│   │\n"
        << "│   ├── character\n"
        << "│   │   ├── -s, --set <set>\n"
        << "│   │   │   ├── alnum\n"
        << "│   │   │   ├── num\n"
        << "│   │   │   ├── upper\n"
        << "│   │   │   ├── lower\n"
        << "│   │   │   ├── hex\n"
        << "│   │   │   ├── base64\n"
        << "│   │   │   └── <custom>\n"
        << "│   │   │       ├── literal\n"
        << "│   │   │       └── ranges\n"
        << "│   │   │\n"
        << "│   │   ├── -l, --length <length>\n"
        << "│   │   ├── -c, --count <count>\n"
        << "│   │   └── --sep <separator>\n"
        << "│   │\n"
        << "│   ├── number\n"
        << "│   │   ├── --range <start> <end>\n"
        << "│   │   └── -c, --count <count>\n"
        << "│   │\n"
        << "│   ├── file\n"
        << "│   │   ├── -f, --file <path>\n"
        << "│   │   ├── -c, --count <count>\n"
        << "│   │   └── --sep <separator>\n"
        << "│   │\n"
        << "│   └── bytes\n"
        << "│       ├── --bytes <count>\n"
        << "│       ├── --hex\n"
        << "│       ├── --base64\n"
        << "│       └── -c, --count <count>\n"
        << "│\n"
        << "├── library\n"
        << "│   └── <mykeygen/mykeygen.hpp>\n"
        << "│       ├── bytes()\n"
        << "│       ├── hex()\n"
        << "│       ├── base64()\n"
        << "│       ├── range()\n"
        << "│       ├── string()\n"
        << "│       ├── alnum()\n"
        << "│       ├── numeric()\n"
        << "│       ├── upper()\n"
        << "│       ├── lower()\n"
        << "│       ├── hexadecimal()\n"
        << "│       └── base64_string()\n"
        << "│\n"
        << "├── output\n"
        << "│   ├── stdout\n"
        << "│   │   └── default\n"
        << "│   ├── -o <file>\n"
        << "│   │   └── overwrite/create\n"
        << "│   └── -oa <file>\n"
        << "│       └── append/create\n"
        << "│\n"
        << "└── custom-set\n"
        << "    ├── A-Z\n"
        << "    ├── a-z\n"
        << "    ├── 0-9\n"
        << "    ├── literal characters\n"
        << "    └── mixed ranges + literals\n";
}