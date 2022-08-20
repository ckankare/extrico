#pragma once

#include <array>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "meta.hpp"
#include "token.hpp"

namespace eto {

template <typename... Ts>
constexpr std::pair<std::array<char, sizeof...(Ts)>, TokenType> make_symbol(TokenType token, Ts... chars) {
    static_assert(utils::are_same_v<Ts..., char>, "Must be chars!");
    static_assert(sizeof...(Ts) > 1, "Must contain at least 2 chars!");
    return {utils::make_array(chars...), token};
}

class Scanner;
class Lexer {
public:
    explicit Lexer(Scanner& scanner);
    Token next();
    void consume() { (void)next(); }

    template <typename... Args>
    std::optional<Token> next_if_any(Args&&... token_types) {
        const auto array = utils::make_array(std::forward<Args>(token_types)...);
        return next_if_any(array);
    }

    template <std::size_t N>
    std::optional<Token> next_if_any(const std::array<TokenType, N>& token_types) {
        auto token = next();
        if (std::any_of(token_types.begin(), token_types.end(), [&](auto type) { return token.type() == type; })) {
            return token;
        } else {
            // Push back to stack.
            m_stack.push_back(std::move(token));
        }
        return {};
    }

    template <typename... Args>
    std::optional<std::array<Token, sizeof...(Args)>> next_if(Args&&... token_types) {
        const auto array = utils::make_array(std::forward<Args>(token_types)...);
        return next_if(array);
    }

    template <std::size_t N>
    std::optional<std::array<Token, N>> next_if(const std::array<TokenType, N>& token_types) {
        std::array<Token, N> stack;
        for (std::size_t i = 0; i < N; ++i) {
            auto token = next();
            if (token.type() == token_types[i]) {
                stack[i] = std::move(token);
            } else {
                // First move the last token
                m_stack.push_back(std::move(token));
                // Then move rest, in the reverse order
                std::move(std::next(stack.rbegin(), static_cast<std::ptrdiff_t>(N - i)), stack.rend(),
                          std::back_inserter(m_stack));
                return {};
            }
        }
        return stack;
    }

    std::optional<Token> next_if(TokenType token_type) {
        auto result = next_if<1>({token_type});
        if (result.has_value()) {
            return {std::move((*result)[0])};
        }
        return {};
    }

    template <std::size_t N>
    std::array<Token, N> look_ahead() {
        static_assert(N != 0, "N can't be 0");
        std::array<Token, N> ret;
        for (std::size_t i = 0; i < N; ++i) {
            ret[i] = next();
        }
        for (std::size_t i = 0; i < N; ++i) {
            if (ret[N - i - 1].type() == TokenType::EndOfStream) {
                continue;
            }
            m_stack.push_back(ret[N - i - 1]);
        }
        return ret;
    }

    Token look_ahead() { return look_ahead<1>()[0]; }
    void revert(Token token) { // :TODO/:HACK
        m_stack.push_back(std::move(token));
    }

    static constexpr std::array<std::pair<std::array<char, 2>, TokenType>, 6> long_symbols = {
        make_symbol(TokenType::GreaterEqual, '>', '='), make_symbol(TokenType::LessEqual, '<', '='),
        make_symbol(TokenType::And, '&', '&'),          make_symbol(TokenType::Or, '|', '|'),
        make_symbol(TokenType::RightArrow, '=', '>'),   make_symbol(TokenType::Equals, '=', '='),
    };

    static constexpr std::array<std::pair<char, TokenType>, 14> symbols = {{
        {';', TokenType::Semicolon},
        {'(', TokenType::LeftParenthesis},
        {')', TokenType::RightParenthesis},
        {'{', TokenType::LeftCurly},
        {'}', TokenType::RightCurly},
        {'=', TokenType::Assign},
        {'+', TokenType::Plus},
        {'-', TokenType::Minus},
        {'*', TokenType::Asterisk},
        {'/', TokenType::Slash},
        {'!', TokenType::Exclamation},
        {',', TokenType::Comma},
        {'@', TokenType::At},
        {'$', TokenType::DollarSign},
    }};
    static constexpr std::array<char, 4> whitespace_char = {' ', '\t', '\n', '\r'};
    static constexpr std::array<std::pair<std::string_view, TokenType>, 1> keywords = {{{"struct", TokenType::Struct}}};
    static constexpr bool is_whitespace(char character);
    static constexpr bool is_letter(char character);
    static constexpr bool is_numeric(char character);
    static constexpr std::optional<TokenType> is_symbol(char character);
    static constexpr std::optional<TokenType> is_keyword(std::string_view str);

private:
    std::vector<Token> m_stack;
    Scanner* m_scanner;
};
} // namespace eto
