#include "charset.hpp"

#include <string>
#include <stdexcept>

[[noreturn]]
static void charset_error(
    const std::string& message)
{
    throw std::runtime_error(message);
}

std::string expand_charset(
    const std::string& expression)
{
    if (expression.empty()) {
        charset_error(
            "character set cannot be empty"
        );
    }

    std::string result;

    for (size_t i = 0;
         i < expression.size();
         ++i) {

        const char current =
            expression[i];

        /*
         * Detect:
         *
         * A-Z
         * a-z
         * 0-9
         *
         * Invalid backwards ranges are treated
         * as literal characters.
         */
        if (i + 2 < expression.size() &&
            expression[i + 1] == '-') {

            const char start =
                expression[i];

            const char end =
                expression[i + 2];

            if (start <= end) {

                for (unsigned int c =
                         static_cast<unsigned char>(
                             start
                         );
                     c <=
                         static_cast<unsigned char>(
                             end
                         );
                     ++c) {

                    result.push_back(
                        static_cast<char>(c)
                    );
                }

                i += 2;
                continue;
            }
        }

        result.push_back(current);
    }

    if (result.empty()) {
        charset_error(
            "character set cannot be empty"
        );
    }

    /*
     * Remove duplicates.
     */
    std::string unique;

    for (char c : result) {

        if (unique.find(c) ==
            std::string::npos) {

            unique.push_back(c);
        }
    }

    return unique;
}

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

    /*
     * Base64 alphabet.
     *
     * '=' is padding, not a random alphabet
     * character, so it is intentionally omitted.
     */
    if (set == "base64") {
        return expand_charset(
            "A-Za-z0-9+/"
        );
    }

    return expand_charset(set);
}