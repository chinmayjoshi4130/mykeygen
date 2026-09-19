#include "generators.hpp"
#include "options.hpp"
#include "output.hpp"
#include "random.hpp"

#include <iostream>

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

            case Mode::Bytes:

                generate_bytes_mode(
                    random,
                    options,
                    output
                );

                break;
        }

        return 0;

    } catch (
        const std::exception& exception
    ) {

        std::cerr
            << "Error: "
            << exception.what()
            << '\n';

        return 1;
    }
}