#pragma once

#include "options.hpp"

#include <fstream>
#include <ostream>

class Output {
public:
    explicit Output(
        const Options& options
    );

    std::ostream& stream();

private:
    std::ofstream file_;
    std::ostream* stream_ = nullptr;
};