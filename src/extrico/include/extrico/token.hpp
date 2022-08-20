#pragma once

#include <algorithm>
#include <ostream>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "meta.hpp"

namespace eto {
enum class TokenType {
    Struct,

    Assign,
    Literal,
    Identifier,
    StringLiteral,

    Equals,
    Plus,
    Minus,
    Asterisk,
    Slash,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    Exclamation,

    Comma,
    Colon,
    Semicolon,
    At,
    DollarSign,

    LeftCurly,
    RightCurly,
    LeftParenthesis,
    RightParenthesis,

    RightArrow,

    And,
    Or,
    BitwiseAnd,
    BitwiseOr,

    EndOfStream,
};

constexpr std::string_view to_string(TokenType type) {
    switch (type) {
        case TokenType::Struct: return "Struct";
        case TokenType::Assign: return "Assign";
        case TokenType::Literal: return "Literal";
        case TokenType::Identifier: return "Identifier";
        case TokenType::StringLiteral: return "StringLiteral";
        case TokenType::Equals: return "Equals";
        case TokenType::Plus: return "Plus";
        case TokenType::Minus: return "Minus";
        case TokenType::Asterisk: return "Asterisk";
        case TokenType::Slash: return "Slash";
        case TokenType::Less: return "Less";
        case TokenType::LessEqual: return "LessEqual";
        case TokenType::Greater: return "Greater";
        case TokenType::GreaterEqual: return "GreaterEqual";
        case TokenType::Exclamation: return "Exclamation";
        case TokenType::Comma: return "Comma";
        case TokenType::Colon: return "Colon";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::At: return "At";
        case TokenType::DollarSign: return "DollarSign";
        case TokenType::LeftCurly: return "LeftCurly";
        case TokenType::RightCurly: return "RightCurly";
        case TokenType::LeftParenthesis: return "LeftParenthesis";
        case TokenType::RightParenthesis: return "RightParenthesis";
        case TokenType::RightArrow: return "RightArrow";
        case TokenType::And: return "And";
        case TokenType::Or: return "Or";
        case TokenType::BitwiseAnd: return "BitwiseAnd";
        case TokenType::BitwiseOr: return "BitwiseOr";
        case TokenType::EndOfStream: return "EndOfStream";
    }
}

class Token {
public:
    Token() : m_type(TokenType::EndOfStream) {}
    explicit Token(TokenType type) : m_type(type) {}
    Token(TokenType type, std::string value) : m_type(type), m_value(std::move(value)) {}

    friend bool operator==(const Token& lhs, const Token& rhs) {
        return lhs.m_type == rhs.m_type && lhs.m_value == rhs.m_value;
    }

    constexpr bool is_end() const { return m_type == TokenType::EndOfStream; }

    template <std::size_t N>
    constexpr bool is(const std::array<TokenType, N>& array) {
        return std::any_of(array.begin(), array.end(), [this](auto v) { return v == m_type; });
    }

    template <typename... Args>
    constexpr bool is(Args&&... args) const {
        static_assert(utils::are_same<TokenType, Args...>::value, "All must be TokenType");
        const auto array = utils::make_array(std::forward<Args>(args)...);
        return is(array);
    }

    constexpr TokenType type() const { return m_type; }
    std::string_view value() const { return m_value; }

private:
    TokenType m_type;
    std::string m_value;
};

std::ostream& operator<<(std::ostream& os, const Token& value);

} // namespace eto

template <>
struct fmt::formatter<eto::TokenType> : fmt::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(eto::TokenType type, FormatContext& ctx) const {
        return fmt::formatter<std::string_view>::format(to_string(type), ctx);
    }
};
