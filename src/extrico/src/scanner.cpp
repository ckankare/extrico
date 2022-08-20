#include "scanner.hpp"

namespace eto {

character_t Scanner::next() {
    if (!has_next()) {
        return character_t();
    }

    position_t pos = m_next;
    character_t ret{pos, m_src[m_next]};

    ++m_next;

    return ret;
}

std::optional<character_t> Scanner::next_if(char character) {
    if (!has_next() || m_src[m_next] != character) {
        return {};
    }
    return next();
}

bool Scanner::next_is(char character) const {
    if (!has_next()) {
        return false;
    }
    return m_src[m_next] == character;
}

bool Scanner::next_is(std::string_view str) const {
    if (m_next + str.length() - 1 >= m_src.length()) {
        return false;
    }
    for (std::size_t i = 0; i < str.length(); ++i) {
        if (m_src[m_next + i] != str[i]) {
            return false;
        }
    }
    return true;
}

std::optional<char> Scanner::peek() const {
    if (!has_next()) {
        return {};
    }
    return {m_src[m_next]};
}

std::optional<std::string_view> Scanner::peek(std::size_t count) const {
    if (m_next + count - 1 >= m_src.length()) {
        return {};
    }
    return std::string_view(m_src).substr(m_next, count);
}

bool Scanner::has_next() const {
    if (m_src.length() <= m_next) {
        return false;
    }
    return true;
}
} // namespace eto
