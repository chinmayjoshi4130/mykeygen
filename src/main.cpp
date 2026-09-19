#include "generators.hpp"
#include "mykeygen/mykeygen.hpp"
#include "options.hpp"
#include "output.hpp"

#include "charset.hpp"
#include "random.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::string join(
    const std::vector<std::string>& values,
    const std::string& separator)
{
    std::string result;

    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0) {
            result += separator;
        }

        result += values[i];
    }

    return result;
}

void run_character(
    const Options& options)
{
    RandomSource random;

    const std::string charset =
        resolve_charset(options.charset);

    std::vector<std::string> values;
    values.reserve(options.count);

    for (std::size_t i = 0; i < options.count; ++i) {
        values.push_back(
            generate_string(
                random,
                charset,
                options.length));
    }

    Output output(
        options.output_mode,
        options.output_file);

    if (!options.separator.empty()) {
        output.write(
            join(values, options.separator));
    } else {
        output.write_lines(values);
    }
}

void run_range(
    const Options& options)
{
    RandomSource random;

    std::vector<std::string> values;
    values.reserve(options.count);

    for (std::size_t i = 0; i < options.count; ++i) {
        values.push_back(
            std::to_string(
                random.range(
                    options.range_min,
                    options.range_max)));
    }

    Output output(
        options.output_mode,
        options.output_file);

    if (!options.separator.empty()) {
        output.write(
            join(values, options.separator));
    } else {
        output.write_lines(values);
    }
}

void run_file(
    const Options& options)
{
    const auto values =
        generate_words(
            options.file,
            options.count);

    Output output(
        options.output_mode,
        options.output_file);

    if (!options.separator.empty()) {
        output.write(
            join(values, options.separator));
    } else {
        output.write_lines(values);
    }
}

void run_bytes(
    const Options& options)
{
    Output output(
        options.output_mode,
        options.output_file);

    for (std::size_t i = 0; i < options.count; ++i) {
        std::string value;

        if (options.byte_encoding == ByteEncoding::Hex) {
            value = mykeygen::hex(
                options.byte_count);
        } else {
            value = mykeygen::base64(
                options.byte_count);
        }

        output.write(value);
    }
}

} // namespace

int main(
    int argc,
    char** argv)
{
    try {
        const Options options =
            parse_options(argc, argv);

        if (options.help) {
            print_help();
            return 0;
        }

        if (options.api) {
            print_api();
            return 0;
        }

        validate_options(options);

        switch (options.mode) {
        case Mode::Character:
            run_character(options);
            break;

        case Mode::Range:
            run_range(options);
            break;

        case Mode::File:
            run_file(options);
            break;

        case Mode::Bytes:
            run_bytes(options);
            break;
        }

        return 0;
    }
    catch (const std::exception& error) {
        std::cerr
            << "mykeygen: "
            << error.what()
            << '\n';

        return 1;
    }
}