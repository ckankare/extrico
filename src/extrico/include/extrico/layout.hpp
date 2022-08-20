#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace eto {

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

class Layout {
public:
    void add_member(Member member);

private:
    std::vector<Member> m_members;
};
} // namespace eto
