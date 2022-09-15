#include "parse.hpp"

namespace eto {
namespace {
    constexpr bool is_valid_hex(char value) {
        return (value >= '0' && value <= '9') || (value >= 'A' && value <= 'F') || (value >= 'a' && value <= 'f');
    }

    constexpr size_t next_valid_hex(std::string_view str, size_t start) {
        for (size_t i = start; i < str.size(); ++i) {
            if (is_valid_hex(str[i])) {
                return i;
            }
        }
        return str.size();
    }

    constexpr uint8_t from_hex(char value) {
        if (value >= '0' && value <= '9') {
            return value - '0';
        }
        if (value >= 'a' && value <= 'f') {
            return value - 'a' + 10;
        }
        if (value >= 'A' && value <= 'F') {
            return value - 'A' + 10;
        }
        return 0;
    }

    constexpr bool is_header_hex(std::string_view str, size_t start) {
        if (str.size() <= start + 1) {
            return false;
        }
        return str[start] == '0' && str[start + 1] == 'x';
    }
} // namespace

std::vector<uint8_t> parse_hex_string(std::string_view str) {
    std::vector<uint8_t> result;

    size_t index = 0;
    index = next_valid_hex(str, index);
    if (is_header_hex(str, index)) {
        index += 2;
    }

    while (index < str.size() - 1) {
        auto upper = from_hex(str[index]);

        if (!is_valid_hex(str[index + 1])) {
            result.push_back(upper * 16);
            break;
        }

        auto lower = from_hex(str[index + 1]);
        result.push_back(upper * 16 + lower);

        index = next_valid_hex(str, index + 2);

        if (is_header_hex(str, index)) {
            index += 2;
        }
    }

    return result;
}
} // namespace eto
