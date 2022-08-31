#include "parser.hpp"

#include <cassert>
#include <charconv>
#include <optional>

#include "lexer.hpp"
#include "scanner.hpp"

namespace eto {
namespace {
    std::optional<uint8_t> parse_digits(std::string_view digits) {
        int32_t value;
        auto [ptr, ec] = std::from_chars(digits.data(), digits.data() + digits.size(), value);
        if (value <= 0 || value > 64 || ec != std::errc()) {
            return {};
        }
        return static_cast<uint8_t>(value);
    }
    std::optional<Type> parse_type(std::string_view type) {
        if (type.size() < 2) {
            return {};
        }

        const auto digits = parse_digits(type.substr(1));
        if (!digits) {
            return {};
        }
        switch (type[0]) {
            case 'b': return Type{BaseType::Boolean, *digits};
            case 'u': return Type{BaseType::Unsigned, *digits};
            case 'i': return Type{BaseType::Signed, *digits};
            case 'f':
                if (*digits != 32 || *digits != 64) {
                    return {};
                }
                return Type{BaseType::Float, *digits};
        }
        // TODO Array?
        // TODO Defined types?

        return {};
    }
    std::unique_ptr<Layout> parse_layout(Lexer& lexer) {
        if (!lexer.consume_any(TokenType::Layout, TokenType::Struct)) {
            return {};
        }
        auto name = lexer.next();
        if (!name.is(TokenType::Identifier)) {
            return {};
        }

        auto layout = std::make_unique<Layout>(std::string(name.value()));

        if (!lexer.consume_any(TokenType::LeftCurly)) {
            return layout;
        }

        do {
            auto identifier = lexer.next_if(TokenType::Identifier);
            if (!identifier || !lexer.consume_any(TokenType::Colon)) {
                break;
            }
            auto type = lexer.next_if(TokenType::Identifier);
            if (!type) {
                break;
            }
            auto parsed_type = parse_type(type->value());
            if (!parsed_type) {
                break;
            }
            layout->add_member(Member{std::string(identifier->value()), std::move(*parsed_type)});

            if (!lexer.consume_any(TokenType::Semicolon)) {
                break;
            }

        } while (true);

        if (!lexer.consume_any(TokenType::RightCurly)) {
            return layout;
        }

        return layout;
    }
} // namespace
std::unique_ptr<Layout> Parser::parse() const {
    Scanner scanner(m_source);
    Lexer lexer(scanner);

    auto layout = parse_layout(lexer);

    return layout;
}
} // namespace eto
