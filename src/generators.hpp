#pragma once

#include "random.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

std::string generate_string(
    RandomSource& random,
    const std::string& charset,
    std::size_t length);

std::vector<std::uint64_t> generate_numbers(
    RandomSource& random,
    std::uint64_t min,
    std::uint64_t max,
    std::size_t count);

std::vector<std::string> generate_words(
    const std::string& path,
    std::size_t count);

std::vector<std::uint8_t> generate_bytes(
    RandomSource& random,
    std::size_t count);

std::string encode_hex(
    const std::vector<std::uint8_t>& data);

std::string encode_base64(
    const std::vector<std::uint8_t>& data);