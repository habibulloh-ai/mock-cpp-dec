# codec

A tiny, dependency-free C++17 command-line tool for encoding and decoding text in the formats developers actually run into every day.

Supports:

| Format | What it does | Where you see it |
| --- | --- | --- |
| **Base64** | Turns any bytes into safe ASCII text. | JWT tokens, email attachments, `data:` image URLs, API keys |
| **Hex** | Writes each byte as two hex characters. | Color codes, hashes, MAC addresses, binary inspection |
| **URL** | Percent-encodes unsafe characters. | Query strings, search URLs, form submissions |

No external libraries. Single source file. Builds anywhere CMake and a C++17 compiler exist.

## Build

Requires CMake 3.10+ and a C++17 compiler (g++, clang, or MSVC).

```bash
cmake -B build
cmake --build build
```

The binary is written to `build/codec` (or `build\Debug\codec.exe` on Windows).

## Usage

```
codec <encode|decode> <base64|hex|url> <input>
```

### Base64

```bash
$ ./build/codec encode base64 "hello world"
aGVsbG8gd29ybGQ=

$ ./build/codec decode base64 "aGVsbG8gd29ybGQ="
hello world
```

### Hex

```bash
$ ./build/codec encode hex "hello"
68656c6c6f

$ ./build/codec decode hex "68656c6c6f"
hello
```

### URL (percent) encoding

```bash
$ ./build/codec encode url "hello world & friends"
hello%20world%20%26%20friends

$ ./build/codec decode url "hello%20world%20%26%20friends"
hello world & friends
```

## When would I actually use this?

- **Base64** — peek at a JWT payload, decode a `data:image/...` URL, build a basic-auth header by hand.
- **Hex** — read a checksum, decode a packet capture, inspect binary data without opening a hex editor.
- **URL** — assemble a search link, debug a malformed query string, encode a filename for a download URL.

## Project layout

```
.
├── CMakeLists.txt
├── README.md
├── .gitignore
└── src/
    └── main.cpp
```

One source file. Each format lives in its own namespace, so the code stays easy to read.

## Adding a new format

Want ROT13, Morse, Caesar cipher, or something exotic? Three steps:

1. Add a `namespace yourformat { encode(); decode(); }` block in `src/main.cpp`.
2. Add an `else if (format == "yourformat")` branch in `main()`.
3. Update the usage string and this README.

## Exit codes

| Code | Meaning |
| --- | --- |
| `0` | Success — result is on stdout |
| `1` | Bad arguments, unknown mode/format, or malformed input |

## License

MIT
