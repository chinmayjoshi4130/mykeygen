#include "charset.hpp"

#include <stdexcept>
#include <string>

namespace {

void append_range(
    std::string& result,
    char first,
    char last)
{
    if (first > last) {
        throw std::invalid_argument(
            "invalid character range");
    }

    for (unsigned int c =
             static_cast<unsigned char>(first);
         c <= static_cast<unsigned char>(last);
         ++c) {
        result.push_back(static_cast<char>(c));
    }
}

} // namespace

std::string expand_charset(
    const std::string& specification)
{
    std::string result;

    for (std::size_t i = 0; i < specification.size();) {
        if (
            i + 2 < specification.size() &&
            specification[i + 1] == '-'
        ) {
            append_range(
                result,
                specification[i],
                specification[i + 2]);

            i += 3;
        } else {
            result.push_back(specification[i]);
            ++i;
        }
    }

    if (result.empty()) {
        throw std::invalid_argument(
            "character set cannot be empty");
    }

    return result;
}

std::string resolve_charset(
    const std::string& specification)
{
    if (specification == "alnum") {
        return
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789";
    }

    if (specification == "num") {
        return "0123456789";
    }

    if (specification == "upper") {
        return "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    }

    if (specification == "lower") {
        return "abcdefghijklmnopqrstuvwxyz";
    }

    if (specification == "hex") {
        return "0123456789abcdef";
    }

    if (specification == "base64") {
        return
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";
    }

    return expand_charset(specification);
}