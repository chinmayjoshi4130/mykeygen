#pragma once

#include "options.hpp"
#include "output.hpp"
#include "random.hpp"

void generate_string_mode(
    RandomSource& random,
    const Options& options,
    Output& output
);

void generate_range_mode(
    RandomSource& random,
    const Options& options,
    Output& output
);

void generate_file_mode(
    RandomSource& random,
    const Options& options,
    Output& output
);

void generate_bytes_mode(
    RandomSource& random,
    const Options& options,
    Output& output
);