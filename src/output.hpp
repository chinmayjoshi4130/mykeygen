#pragma once

#include <string>
#include <vector>

enum class OutputMode {
    Stdout,
    Overwrite,
    Append
};

class Output {
public:
    Output(
        OutputMode mode,
        const std::string& path = "");

    void write(const std::string& value);

    void write_lines(
        const std::vector<std::string>& values,
        const std::string& separator = "");

private:
    OutputMode mode_;
    std::string path_;
};