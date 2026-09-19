#include "output.hpp"

#include <stdexcept>
#include <iostream>
#include <string>

Output::Output(
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
                throw std::runtime_error(
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
                throw std::runtime_error(
                    "cannot open output file: " +
                    options.output_file
                );
            }

            stream_ = &file_;

            break;
    }
}

std::ostream& Output::stream()
{
    return *stream_;
}