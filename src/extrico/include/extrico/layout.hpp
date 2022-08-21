#pragma once

#include <memory>
#include <span>
#include <string>
#include <variant>
#include <vector>

namespace eto {

enum class Endianess { Little, Big };
enum class BaseType { Float, Signed, Unsigned, Boolean };

struct Type {
    BaseType base;
    uint8_t bit_width;
};

class Layout;

struct Member {
    std::string name;
    std::variant<Type, Layout*> type;
};

struct Value {
    Type member;
    uint64_t bits;
};

class Layout {
public:
    void add_member(Member member);
    uint64_t bit_width() const { return m_total_bit_width; }

    std::pair<std::vector<Value>, size_t> parse_bits(std::span<uint8_t> data, Endianess endianess) const;

private:
    std::vector<Member> m_members;
    size_t m_total_bit_width = 0;
};
} // namespace eto
