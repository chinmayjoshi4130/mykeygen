#include "output.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

Output::Output(
    OutputMode mode,
    const std::string& path)
    : mode_(mode),
      path_(path)
{
    if (
        mode_ != OutputMode::Stdout &&
        path_.empty()
    ) {
        throw std::invalid_argument(
            "output file path is required");
    }
}

void Output::write(
    const std::string& value)
{
    if (mode_ == OutputMode::Stdout) {
        std::cout << value << '\n';
        return;
    }

    const auto open_mode =
        mode_ == OutputMode::Append
            ? std::ios::app
            : std::ios::trunc;

    std::ofstream file(
        path_,
        std::ios::out | open_mode);

    if (!file) {
        throw std::runtime_error(
            "failed to open output file: " + path_);
    }

    file << value << '\n';

    if (!file) {
        throw std::runtime_error(
            "failed to write output file: " + path_);
    }
}

void Output::write_lines(
    const std::vector<std::string>& values,
    const std::string& separator)
{
    if (mode_ == OutputMode::Stdout) {
        for (const auto& value : values) {
            std::cout << value << '\n';
        }

        return;
    }

    const auto open_mode =
        mode_ == OutputMode::Append
            ? std::ios::app
            : std::ios::trunc;

    std::ofstream file(
        path_,
        std::ios::out | open_mode);

    if (!file) {
        throw std::runtime_error(
            "failed to open output file: " + path_);
    }

    if (!separator.empty()) {
        for (std::size_t i = 0; i < values.size(); ++i) {
            if (i != 0) {
                file << separator;
            }

            file << values[i];
        }

        file << '\n';
    } else {
        for (const auto& value : values) {
            file << value << '\n';
        }
    }

    if (!file) {
        throw std::runtime_error(
            "failed to write output file: " + path_);
    }
}