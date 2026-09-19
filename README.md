# mykeygen

A small cross-platform C++17 utility and library for generating
cryptographically secure random passwords, secrets, tokens, bytes,
PINs, and random values.

mykeygen is intentionally small. It provides secure random generation
while leaving cryptographic operations such as encryption, hashing,
and key derivation to established libraries such as OpenSSL or
libsodium.

## Features

- Cryptographically secure random bytes
- Secure random integer ranges
- Random passwords and strings
- Predefined character sets
- Custom character sets and ranges
- Numeric PIN generation
- Hexadecimal encoding
- Base64 encoding
- File-based word generation
- CLI and reusable C++ library
- POSIX/Linux/Android support
- Windows support
- No external runtime dependencies

## Example

Generate a random password:

```bash
mykeygen --set 'A-Za-z0-9!@#$%^&*' --length 32
```

Generate a six-digit PIN:

```bash
mykeygen --set num --length 6
```

Generate 32 random bytes as hexadecimal:

```bash
mykeygen --bytes 32 --hex
```

Generate random bytes as Base64:

```bash
mykeygen --bytes 32 --base64
```

Generate a value from an inclusive range:

```bash
mykeygen --range 100000 999999
```

Generate multiple values:

```bash
mykeygen --set alnum --length 32 --count 5
```

## Character Sets

The following predefined character sets are available:

| Set | Characters |
|---|---|
| `alnum` | Uppercase, lowercase, and digits |
| `num` | Digits |
| `upper` | Uppercase letters |
| `lower` | Lowercase letters |
| `hex` | `0-9a-f` |
| `base64` | Base64 alphabet |

Custom character sets are also supported.

```bash
mykeygen --set 'A-Za-z0-9!@#$%' --length 32
```

Character ranges such as `A-Z`, `a-z`, and `0-9` can be combined
with literal characters.

## Output

Generated values can be written to standard output or directly to
a file.

Overwrite or create a file:

```bash
mykeygen --set alnum --length 32 -o secret.txt
```

Append to a file:

```bash
mykeygen --set alnum --length 32 -oa secrets.txt
```

Multiple generated values can be separated using a custom separator:

```bash
mykeygen --set alnum --length 32 --count 5 --sep '\n'
```

## Library

mykeygen can also be used as a C++17 library.

Include the public header:

```cpp
#include <mykeygen/mykeygen.hpp>
```

Generate a password:

```cpp
const auto password =
    mykeygen::alnum(32);
```

Generate random bytes:

```cpp
const auto secret =
    mykeygen::bytes(32);
```

Encode the same bytes as hexadecimal:

```cpp
const auto hex =
    mykeygen::hex(secret);
```

Encode the same bytes as Base64:

```cpp
const auto base64 =
    mykeygen::base64(secret);
```

Generate a random range:

```cpp
const auto pin =
    mykeygen::range(100000, 999999);
```

Generate a custom string:

```cpp
const auto value =
    mykeygen::string(
        "ABCDEFGHJKLMNPQRSTUVWXYZ23456789",
        20);
```

## Security

mykeygen obtains randomness directly from the operating system's
cryptographically secure random number generator.

### POSIX

On Linux, Android/Termux, macOS, and other POSIX systems, mykeygen
uses:

```text
/dev/urandom
```

The implementation performs exact reads and handles interrupted
reads.

### Windows

On Windows, mykeygen uses:

```cpp
BCryptGenRandom(
    nullptr,
    buffer,
    size,
    BCRYPT_USE_SYSTEM_PREFERRED_RNG
);
```

The platform backend is selected at build time.

### Random Ranges

Random integer generation uses rejection sampling rather than a
simple modulo operation to avoid modulo bias.

## What mykeygen Does Not Do

mykeygen is a random-material generator, not a complete cryptographic
library.

It does not provide:

- Password storage
- Password vaults
- Encryption
- Hashing
- Password-based key derivation
- Authentication protocols
- Digital signatures
- Key exchange
- Secure file transfer

For these operations, use established cryptographic libraries such
as OpenSSL or libsodium.

For example, a password manager can use mykeygen for random salts or
generated passwords while using a dedicated cryptographic library
for encryption and key derivation.

## Project Structure

```text
mykeygen/
├── Makefile
├── include/
│   └── mykeygen/
│       └── mykeygen.hpp
├── examples/
│   └── basic.cpp
└── src/
    ├── charset.cpp
    ├── charset.hpp
    ├── generators.cpp
    ├── generators.hpp
    ├── main.cpp
    ├── mykeygen.cpp
    ├── options.cpp
    ├── options.hpp
    ├── output.cpp
    ├── output.hpp
    ├── random.cpp
    ├── random.hpp
    ├── random_posix.cpp
    └── random_windows.cpp
```

## Building

Requirements:

- C++17 compiler
- Make

Build the CLI and static library:

```bash
make
```

The resulting files are:

```text
build/mykeygen
build/libmykeygen.a
```

Build only the library:

```bash
make library
```

Clean the build:

```bash
make clean
```

Rebuild everything:

```bash
make rebuild
```

## Using the Library

After building, a program can link against the static library:

```bash
clang++ -std=c++17 \
    -Iinclude \
    examples/basic.cpp \
    build/libmykeygen.a \
    -o example
```

Run it:

```bash
./example
```

## Installation

The default installation prefix is `/usr/local`.

```bash
make install
```

This installs:

```text
/usr/local/bin/mykeygen
/usr/local/lib/libmykeygen.a
/usr/local/include/mykeygen/mykeygen.hpp
```

A different prefix can be selected:

```bash
make PREFIX="$HOME/.local" install
```

Uninstall:

```bash
make uninstall
```

## Intended Use

mykeygen is useful when a program or developer needs a secure random
value without needing a full cryptographic framework.

Typical uses include:

- Account password generation
- Random passphrases
- One-time secrets
- API tokens
- Authentication tokens
- Random salts
- Session identifiers
- Temporary secrets
- Random PINs
- Application-generated keys
- Secure random values for other programs

For example:

```text
mykeygen
    │
    ├── password generation
    ├── secret generation
    ├── PIN generation
    ├── token generation
    ├── random salts
    └── application randomness
```

Applications requiring encryption, hashing, key derivation, or other
cryptographic operations should use a dedicated cryptographic library.

## License

This project is provided as a personal/open-source utility.
Add the project's chosen license here.

## Status

mykeygen is a small personal project focused on secure random
generation and cross-platform C++ library design.

The project is intentionally kept small rather than attempting to
replace established cryptographic libraries.