#pragma once

#include <string_view>
#include <vector>

namespace eto {

std::vector<uint8_t> parse_hex_string(std::string_view str);
} // namespace eto
