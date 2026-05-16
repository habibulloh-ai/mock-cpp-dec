#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <cctype>

namespace base64 {

static const std::string kChars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

std::string encode(const std::string& input) {
    std::string out;
    int val = 0;
    int valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(kChars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) {
        out.push_back(kChars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    while (out.size() % 4 != 0) {
        out.push_back('=');
    }
    return out;
}

std::string decode(const std::string& input) {
    std::vector<int> table(256, -1);
    for (int i = 0; i < 64; ++i) {
        table[static_cast<unsigned char>(kChars[i])] = i;
    }
    std::string out;
    int val = 0;
    int valb = -8;
    for (unsigned char c : input) {
        if (c == '=' || c == '\n' || c == '\r') break;
        if (table[c] == -1) {
            throw std::runtime_error("invalid base64 character");
        }
        val = (val << 6) + table[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(static_cast<char>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

}  // namespace base64

namespace hex {

std::string encode(const std::string& input) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (unsigned char c : input) {
        oss << std::setw(2) << static_cast<int>(c);
    }
    return oss.str();
}

std::string decode(const std::string& input) {
    if (input.size() % 2 != 0) {
        throw std::runtime_error("hex input must have an even number of characters");
    }
    std::string out;
    out.reserve(input.size() / 2);
    for (size_t i = 0; i < input.size(); i += 2) {
        int byte = 0;
        std::istringstream iss(input.substr(i, 2));
        if (!(iss >> std::hex >> byte)) {
            throw std::runtime_error("invalid hex character");
        }
        out.push_back(static_cast<char>(byte));
    }
    return out;
}

}  // namespace hex

namespace url {

std::string encode(const std::string& input) {
    std::ostringstream oss;
    oss << std::hex << std::uppercase << std::setfill('0');
    for (unsigned char c : input) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            oss << c;
        } else {
            oss << '%' << std::setw(2) << static_cast<int>(c);
        }
    }
    return oss.str();
}

std::string decode(const std::string& input) {
    std::string out;
    out.reserve(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        char c = input[i];
        if (c == '%') {
            if (i + 2 >= input.size()) {
                throw std::runtime_error("invalid percent-encoded sequence");
            }
            int byte = 0;
            std::istringstream iss(input.substr(i + 1, 2));
            if (!(iss >> std::hex >> byte)) {
                throw std::runtime_error("invalid percent-encoded sequence");
            }
            out.push_back(static_cast<char>(byte));
            i += 2;
        } else if (c == '+') {
            out.push_back(' ');
        } else {
            out.push_back(c);
        }
    }
    return out;
}

}  // namespace url

namespace {

void print_usage(std::ostream& os) {
    os << "codec - tiny C++ encoder/decoder for base64, hex, and url formats\n\n"
       << "Usage:\n"
       << "  codec <encode|decode> <base64|hex|url> <input>\n\n"
       << "Examples:\n"
       << "  codec encode base64 \"hello world\"\n"
       << "  codec decode base64 \"aGVsbG8gd29ybGQ=\"\n"
       << "  codec encode hex    \"hello\"\n"
       << "  codec decode url    \"hello%20world\"\n";
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc != 4) {
        print_usage(std::cerr);
        return 1;
    }

    const std::string mode = argv[1];
    const std::string format = argv[2];
    const std::string input = argv[3];

    if (mode != "encode" && mode != "decode") {
        std::cerr << "error: unknown mode '" << mode << "'\n\n";
        print_usage(std::cerr);
        return 1;
    }
    const bool encoding = (mode == "encode");

    try {
        std::string result;
        if (format == "base64") {
            result = encoding ? base64::encode(input) : base64::decode(input);
        } else if (format == "hex") {
            result = encoding ? hex::encode(input) : hex::decode(input);
        } else if (format == "url") {
            result = encoding ? url::encode(input) : url::decode(input);
        } else {
            std::cerr << "error: unknown format '" << format << "'\n\n";
            print_usage(std::cerr);
            return 1;
        }
        std::cout << result << '\n';
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
}
