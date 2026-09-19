#include <mykeygen/mykeygen.hpp>

#include <iostream>

int main()
{
    // Generate one random value.
    const auto session_random =
        mykeygen::bytes(32);

    // Represent the exact same bytes in two formats.
    const auto session_hex =
        mykeygen::hex(session_random);

    const auto session_base64 =
        mykeygen::base64(session_random);

    // Generate a human-readable authentication PIN.
    const auto pin =
        mykeygen::numeric(6);

    std::cout
        << "mykeygen example\n"
        << "----------------\n";

    std::cout
        << "PIN: "
        << pin
        << '\n';

    std::cout
        << "Random bytes: "
        << session_random.size()
        << " bytes\n";

    std::cout
        << "Hex: "
        << session_hex
        << '\n';

    std::cout
        << "Base64: "
        << session_base64
        << '\n';

    return 0;
}