#pragma once

#include <string_view>

#include "layout.hpp"

namespace eto {
class Parser {
public:
    explicit Parser(std::string_view source) : m_source(source) {}

    [[nodiscard]] std::unique_ptr<Layout> parse() const;

private:
    std::string_view m_source;
};
} // namespace eto
