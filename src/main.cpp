#include <cerrno>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>


// ============================================================
// RANDOM SOURCE
// ============================================================

class RandomSource {
public:
    RandomSource()
    {
        fd_ = open("/dev/urandom", O_RDONLY);

        if (fd_ < 0) {
            throw std::runtime_error(
                "cannot open /dev/urandom"
            );
        }
    }

    ~RandomSource()
    {
        if (fd_ >= 0) {
            close(fd_);
        }
    }

    RandomSource(const RandomSource&) = delete;
    RandomSource& operator=(const RandomSource&) = delete;

    uint8_t byte()
    {
        uint8_t value;
        read_exact(&value, sizeof(value));
        return value;
    }

    uint64_t uint64()
    {
        uint64_t value;
        read_exact(&value, sizeof(value));
        return value;
    }

    uint64_t range(uint64_t min, uint64_t max)
    {
        if (min > max) {
            throw std::runtime_error(
                "invalid random range"
            );
        }

        const uint64_t range = max - min + 1;

        // Overflow means the requested range contains
        // every possible uint64_t value.
        if (range == 0) {
            return uint64();
        }

        /*
         * Rejection sampling.
         *
         * Using UINT64_MAX + 1 as the sampling space,
         * we reject the incomplete tail so every result
         * has equal probability.
         */
        const uint64_t limit =
            std::numeric_limits<uint64_t>::max()
            - (
                std::numeric_limits<uint64_t>::max()
                % range
            );

        while (true) {
            const uint64_t value = uint64();

            if (value < limit) {
                return min + (value % range);
            }
        }
    }

private:
    int fd_ = -1;

    void read_exact(void* buffer, size_t size)
    {
        auto* ptr =
            static_cast<unsigned char*>(buffer);

        while (size > 0) {
            const ssize_t result =
                read(fd_, ptr, size);

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
};


// ============================================================
// MODES
// ============================================================

enum class Mode {
    String,
    Range,
    File
};

enum class OutputMode {
    Stdout,
    Overwrite,
    Append
};


// ============================================================
// OPTIONS
// ============================================================

struct Options {
    Mode mode = Mode::String;

    std::string set = "alnum";
    uint64_t length = 16;
    uint64_t count = 1;

    std::string separator;

    int64_t range_start = 0;
    int64_t range_end = 0;

    std::string word_file;

    std::string output_file;
    OutputMode output_mode = OutputMode::Stdout;
};


// ============================================================
// ERROR
// ============================================================

[[noreturn]]
void error(const std::string& message)
{
    std::cerr << "Error: " << message << '\n';
    std::exit(1);
}


// ============================================================
// HELP
// ============================================================

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
)";
}


// ============================================================
// API
// ============================================================

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
│   └── file
│       ├── -f, --file <path>
│       ├── -c, --count <count>
│       └── --sep <separator>
│
├── output
│   │
│   ├── stdout
│   │   └── default
│   │
│   ├── -o <file>
│   │   └── overwrite/create
│   │
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


// ============================================================
// PARSING HELPERS
// ============================================================

bool is_unsigned_integer(const std::string& value)
{
    if (value.empty()) {
        return false;
    }

    for (char c : value) {
        if (!std::isdigit(
                static_cast<unsigned char>(c))) {
            return false;
        }
    }

    return true;
}


uint64_t parse_positive(
    const std::string& value,
    const std::string& name)
{
    if (!is_unsigned_integer(value)) {
        error(name + " must be a positive number");
    }

    try {
        const unsigned long long result =
            std::stoull(value);

        if (result == 0) {
            error(
                name + " must be greater than zero"
            );
        }

        return static_cast<uint64_t>(result);
    }
    catch (...) {
        error(name + " is too large");
    }
}


int64_t parse_integer(
    const std::string& value,
    const std::string& name)
{
    if (value.empty()) {
        error(name + " must be an integer");
    }

    size_t start = 0;

    if (value[0] == '-' ||
        value[0] == '+') {
        start = 1;
    }

    if (start == value.size()) {
        error(name + " must be an integer");
    }

    for (size_t i = start;
         i < value.size();
         ++i) {

        if (!std::isdigit(
                static_cast<unsigned char>(
                    value[i]))) {
            error(name + " must be an integer");
        }
    }

    try {
        return std::stoll(value);
    }
    catch (...) {
        error(name + " is out of range");
    }
}


// ============================================================
// CHARACTER SET EXPRESSION
// ============================================================
//
// Supported:
//
//   abc
//   A-Z
//   a-z
//   0-9
//   A-Za-z0-9
//   A-F0-9!@#$%
//
// '-' is a range operator when:
//
//   previous character exists
//   next character exists
//   previous <= next
//
// Otherwise '-' is treated as a literal.
//
// ============================================================

std::string expand_charset(
    const std::string& expression)
{
    if (expression.empty()) {
        error("character set cannot be empty");
    }

    std::string result;

    for (size_t i = 0;
         i < expression.size();
         ++i) {

        const char current = expression[i];

        // ----------------------------------------------------
        // Range
        // ----------------------------------------------------

        if (i + 2 < expression.size() &&
            expression[i + 1] == '-') {

            const char start = expression[i];
            const char end = expression[i + 2];

            if (start <= end) {

                for (unsigned int c =
                         static_cast<unsigned char>(start);
                     c <= static_cast<unsigned char>(end);
                     ++c) {

                    result.push_back(
                        static_cast<char>(c)
                    );
                }

                i += 2;
                continue;
            }
        }

        // ----------------------------------------------------
        // Literal
        // ----------------------------------------------------

        result.push_back(current);
    }

    if (result.empty()) {
        error("character set cannot be empty");
    }

    // Remove duplicate characters.
    std::string unique;

    for (char c : result) {
        if (unique.find(c) == std::string::npos) {
            unique.push_back(c);
        }
    }

    return unique;
}


// ============================================================
// PREDEFINED CHARACTER SETS
// ============================================================

std::string resolve_set(
    const std::string& set)
{
    if (set == "alnum") {
        return expand_charset(
            "A-Za-z0-9"
        );
    }

    if (set == "num") {
        return expand_charset(
            "0-9"
        );
    }

    if (set == "upper") {
        return expand_charset(
            "A-Z"
        );
    }

    if (set == "lower") {
        return expand_charset(
            "a-z"
        );
    }

    if (set == "hex") {
        return expand_charset(
            "0-9a-f"
        );
    }

    if (set == "base64") {
        return expand_charset(
            "A-Za-z0-9+/="
        );
    }

    return expand_charset(set);
}


// ============================================================
// STRING GENERATOR
// ============================================================

std::string generate_string(
    RandomSource& random,
    const std::string& charset,
    uint64_t length)
{
    if (charset.empty()) {
        error("character set cannot be empty");
    }

    if (length >
        static_cast<uint64_t>(
            std::numeric_limits<size_t>::max())) {
        error("length is too large");
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


// ============================================================
// TRIM
// ============================================================

std::string trim(
    const std::string& value)
{
    size_t begin = 0;
    size_t end = value.size();

    while (begin < end &&
           std::isspace(
               static_cast<unsigned char>(
                   value[begin]))) {
        ++begin;
    }

    while (end > begin &&
           std::isspace(
               static_cast<unsigned char>(
                   value[end - 1]))) {
        --end;
    }

    return value.substr(
        begin,
        end - begin
    );
}


// ============================================================
// LOAD WORD FILE
// ============================================================

std::vector<std::string> load_words(
    const std::string& path)
{
    std::ifstream file(path);

    if (!file) {
        error(
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
        error(
            "word file contains no usable words"
        );
    }

    return words;
}


// ============================================================
// OUTPUT
// ============================================================

class Output {
public:
    explicit Output(
        const Options& options)
    {
        switch (options.output_mode) {

            case OutputMode::Stdout:
                stream_ = &std::cout;
                break;

            case OutputMode::Overwrite:
                file_.open(
                    options.output_file,
                    std::ios::out |
                    std::ios::trunc
                );

                if (!file_) {
                    error(
                        "cannot open output file: " +
                        options.output_file
                    );
                }

                stream_ = &file_;
                break;

            case OutputMode::Append:
                file_.open(
                    options.output_file,
                    std::ios::out |
                    std::ios::app
                );

                if (!file_) {
                    error(
                        "cannot open output file: " +
                        options.output_file
                    );
                }

                stream_ = &file_;
                break;
        }
    }

    std::ostream& stream()
    {
        return *stream_;
    }

private:
    std::ofstream file_;
    std::ostream* stream_ = nullptr;
};


// ============================================================
// ARGUMENT PARSER
// ============================================================

Options parse_arguments(
    int argc,
    char* argv[])
{
    Options options;

    for (int i = 1;
         i < argc;
         ++i) {

        const std::string arg = argv[i];


        // ----------------------------------------------------
        // HELP
        // ----------------------------------------------------

        if (arg == "-h" ||
            arg == "--help" ||
            arg == "help") {

            usage();
            std::exit(0);
        }


        // ----------------------------------------------------
        // API
        // ----------------------------------------------------

        if (arg == "api") {
            api();
            std::exit(0);
        }


        // ----------------------------------------------------
        // SET
        // ----------------------------------------------------

        if (arg == "-s" ||
            arg == "--set") {

            if (options.mode != Mode::String) {
                error(
                    "--set cannot be combined with "
                    "another generator"
                );
            }

            if (++i >= argc) {
                error(
                    arg +
                    " requires a character set"
                );
            }

            options.set = argv[i];

            continue;
        }


        // ----------------------------------------------------
        // LENGTH
        // ----------------------------------------------------

        if (arg == "-l" ||
            arg == "--length") {

            if (options.mode != Mode::String) {
                error(
                    "--length cannot be used with "
                    "another generator"
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


        // ----------------------------------------------------
        // COUNT
        // ----------------------------------------------------

        if (arg == "-c" ||
            arg == "--count") {

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


        // ----------------------------------------------------
        // SEPARATOR
        // ----------------------------------------------------

        if (arg == "--sep") {

            if (++i >= argc) {
                error(
                    "--sep requires a separator"
                );
            }

            options.separator = argv[i];

            continue;
        }


        // ----------------------------------------------------
        // RANGE
        // ----------------------------------------------------

        if (arg == "--range") {

            if (options.mode != Mode::String) {
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

            options.mode = Mode::Range;

            continue;
        }


        // ----------------------------------------------------
        // FILE
        // ----------------------------------------------------

        if (arg == "-f" ||
            arg == "--file") {

            if (options.mode != Mode::String) {
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

            options.word_file = argv[i];

            options.mode = Mode::File;

            continue;
        }


        // ----------------------------------------------------
        // OUTPUT
        // ----------------------------------------------------

        if (arg == "-o") {

            if (++i >= argc) {
                error(
                    "-o requires a file path"
                );
            }

            options.output_file = argv[i];

            options.output_mode =
                OutputMode::Overwrite;

            continue;
        }


        // ----------------------------------------------------
        // OUTPUT APPEND
        // ----------------------------------------------------

        if (arg == "-oa") {

            if (++i >= argc) {
                error(
                    "-oa requires a file path"
                );
            }

            options.output_file = argv[i];

            options.output_mode =
                OutputMode::Append;

            continue;
        }


        // ----------------------------------------------------
        // UNKNOWN
        // ----------------------------------------------------

        error(
            "unknown option: " + arg
        );
    }

    return options;
}


// ============================================================
// VALIDATION
// ============================================================

void validate(
    const Options& options)
{
    // --------------------------------------------------------
    // RANGE
    // --------------------------------------------------------

    if (options.mode == Mode::Range) {

        if (!options.separator.empty()) {
            error(
                "--sep cannot be used with --range"
            );
        }

        if (options.range_start >
            options.range_end) {

            error(
                "range start must be less than "
                "or equal to end"
            );
        }

        return;
    }


    // --------------------------------------------------------
    // FILE
    // --------------------------------------------------------

    if (options.mode == Mode::File) {

        if (options.length != 16) {
            error(
                "--length cannot be used with --file"
            );
        }

        if (options.set != "alnum") {
            error(
                "--set cannot be used with --file"
            );
        }

        if (options.range_start != 0 ||
            options.range_end != 0) {

            error(
                "--range cannot be used with --file"
            );
        }

        return;
    }


    // --------------------------------------------------------
    // STRING
    // --------------------------------------------------------

    const std::string charset =
        resolve_set(options.set);

    if (charset.empty()) {
        error(
            "character set cannot be empty"
        );
    }
}


// ============================================================
// STRING MODE
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


    // --------------------------------------------------------
    // Normal output
    // --------------------------------------------------------

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


    // --------------------------------------------------------
    // Grouped output
    // --------------------------------------------------------

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
// SIGNED RANDOM RANGE
// ============================================================

uint64_t signed_range_size(
    int64_t min,
    int64_t max)
{
    if (min >= 0) {
        return static_cast<uint64_t>(
            max - min
        ) + 1;
    }

    if (max < 0) {
        return static_cast<uint64_t>(
            max - min
        ) + 1;
    }

    // Range crosses zero.
    //
    // Avoid signed overflow by calculating:
    //
    // (-min) + max + 1
    //
    const uint64_t negative =
        static_cast<uint64_t>(
            -(min + 1)
        ) + 1;

    const uint64_t positive =
        static_cast<uint64_t>(max);

    return negative + positive + 1;
}


int64_t random_signed_range(
    RandomSource& random,
    int64_t min,
    int64_t max)
{
    const uint64_t size =
        signed_range_size(min, max);

    const uint64_t offset =
        random.range(0, size - 1);

    if (min >= 0) {
        return min +
            static_cast<int64_t>(offset);
    }

    if (max < 0) {
        return min +
            static_cast<int64_t>(offset);
    }

    // Crossing zero.
    return min +
        static_cast<int64_t>(offset);
}


// ============================================================
// RANGE MODE
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
            << random_signed_range(
                random,
                options.range_start,
                options.range_end
            )
            << '\n';
    }
}


// ============================================================
// FILE MODE
// ============================================================

void generate_file_mode(
    RandomSource& random,
    const Options& options,
    Output& output)
{
    const std::vector<std::string> words =
        load_words(options.word_file);

    const uint64_t last =
        static_cast<uint64_t>(
            words.size() - 1
        );

    std::ostream& out =
        output.stream();


    // --------------------------------------------------------
    // One word per line
    // --------------------------------------------------------

    if (options.separator.empty()) {

        for (uint64_t i = 0;
             i < options.count;
             ++i) {

            const uint64_t index =
                random.range(0, last);

            out
                << words[
                    static_cast<size_t>(index)
                ]
                << '\n';
        }

        return;
    }


    // --------------------------------------------------------
    // Joined words
    // --------------------------------------------------------

    for (uint64_t i = 0;
         i < options.count;
         ++i) {

        if (i > 0) {
            out << options.separator;
        }

        const uint64_t index =
            random.range(0, last);

        out
            << words[
                static_cast<size_t>(index)
            ];
    }

    out << '\n';
}


// ============================================================
// MAIN
// ============================================================

int main(
    int argc,
    char* argv[])
{
    try {

        Options options =
            parse_arguments(
                argc,
                argv
            );

        validate(options);

        RandomSource random;

        Output output(options);

        switch (options.mode) {

            case Mode::String:
                generate_string_mode(
                    random,
                    options,
                    output
                );
                break;

            case Mode::Range:
                generate_range_mode(
                    random,
                    options,
                    output
                );
                break;

            case Mode::File:
                generate_file_mode(
                    random,
                    options,
                    output
                );
                break;
        }

        return 0;
    }
    catch (const std::exception& exception) {

        std::cerr
            << "Error: "
            << exception.what()
            << '\n';

        return 1;
    }
}
