#pragma once

#include <string_view>
#include <vector>

namespace eto {

std::pair<std::vector<uint8_t>, std::vector<size_t>> parse_hex_string(std::string_view str);
} // namespace eto
