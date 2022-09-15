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
    Type type;
    uint64_t bits;
};

class Layout {
public:
    Layout(std::string name) noexcept : m_name(std::move(name)) {}
    std::string_view name() const { return m_name; }
    std::size_t size() const { return m_members.size(); }

    const Member& operator[](std::size_t index) const noexcept { return m_members[index]; }

    void add_member(Member member) noexcept;
    uint64_t bit_width() const noexcept { return m_total_bit_width; }
    std::string to_string(std::span<Value> values) const noexcept;

    std::pair<std::vector<Value>, size_t> parse_bits(std::span<uint8_t> data, Endianess endianess) const noexcept;

private:
    std::string m_name;
    std::vector<Member> m_members;
    size_t m_total_bit_width = 0;
};
} // namespace eto
