#include "lexer.hpp"

#include <cassert>

#include "scanner.hpp"

namespace eto {
Lexer::Lexer(Scanner& scanner) : m_scanner(&scanner) {}
Token Lexer::next() {
    if (!m_stack.empty()) {
        auto token = std::move(m_stack.back());
        m_stack.pop_back();
        return token;
    }
    // Skip whitespaces
    if (!m_scanner->has_next()) {
        return Token(TokenType::EndOfStream);
    }
    auto character = m_scanner->next();
    while (is_whitespace(character.character)) {
        if (!m_scanner->has_next()) {
            return Token(TokenType::EndOfStream);
        }
        character = m_scanner->next();
    }

    const auto c = character.character;

    if (const auto next = m_scanner->peek(); next.has_value()) {
        for (const auto& symbol : long_symbols) {
            if (symbol.first[0] == c && symbol.first[1] == next) {
                m_scanner->next();
                return Token(symbol.second);
            }
        }
    }

    if (auto symbol = is_symbol(c); symbol.has_value()) {
        return Token(*symbol);
    }

    if (is_letter(c) || c == '_') {
        std::string value;
        value += c;
        auto peek = m_scanner->peek();
        while (peek.has_value() && (is_letter(*peek) || is_numeric(*peek) || (*peek) == '_')) {
            m_scanner->next();
            value += *peek;
            peek = m_scanner->peek();
        }
        if (auto keyword = is_keyword(value); keyword.has_value()) {
            return Token(*keyword);
        }
        return Token(TokenType::Identifier, std::move(value));
    }

    if (c == '"') {
        std::string value;
        bool slash = false;
        while (m_scanner->has_next()) {
            auto next = m_scanner->next();
            if (slash) {
                switch (next.character) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    default: value += next.character;
                }
                slash = false;
                continue;
            }
            if (next.character == '"') {
                return {TokenType::StringLiteral, std::move(value)};
            }
            if (next.character == '\\') {
                slash = true;
            } else {
                value += next.character;
            }
        }

        // TODO This is actually an error, which we maybe should handle some other way?
        return {TokenType::StringLiteral, std::move(value)};
    }

    if (is_numeric(c)) {
        std::string value;
        value += c;

        // 0x, 0b, 0o
        if (c == '0' && m_scanner->has_next()) {
            auto peek = m_scanner->peek();
            if (*peek == 'x' || *peek == 'b' || *peek == 'o') {
                value += *peek;
                m_scanner->next();
            }
        }

        auto peek = m_scanner->peek();
        while (peek.has_value() && is_numeric(*peek)) {
            m_scanner->next();
            value += *peek;
            peek = m_scanner->peek();
        }

        if (peek.has_value() && *peek == '.') {
            m_scanner->next();
            value += *peek;
            peek = m_scanner->peek();
            while (peek.has_value() && is_numeric(*peek)) {
                m_scanner->next();
                value += *peek;
                peek = m_scanner->peek();
            }
        }

        return Token(TokenType::Literal, std::move(value));
    }

    return Token(TokenType::EndOfStream);
}

constexpr bool Lexer::is_whitespace(char character) {
    for (const auto w : whitespace_char) {
        if (w == character) {
            return true;
        }
    }
    return false;
}
constexpr bool Lexer::is_letter(char character) {
    return (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z');
}
constexpr bool Lexer::is_numeric(char character) { return (character >= '0' && character <= '9'); }
constexpr std::optional<TokenType> Lexer::is_symbol(char character) {
    for (const auto s : symbols) {
        if (s.first == character) {
            return {s.second};
        }
    }
    return {};
}
constexpr std::optional<TokenType> Lexer::is_keyword(std::string_view str) {
    for (const auto& keyword : keywords) {
        if (str == keyword.first) {
            return {keyword.second};
        }
    }
    return {};
}
} // namespace eto
