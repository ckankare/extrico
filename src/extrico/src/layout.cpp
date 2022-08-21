#include "layout.hpp"

#include <cassert>
#include <iterator>

#include "bits_view.hpp"
#include "meta.hpp"

namespace eto {

void Layout::add_member(Member member) {
    m_total_bit_width +=
        std::visit(utils::overload{[&](const Type& type) { return static_cast<uint64_t>(type.bit_width); },
                                   [&](const Layout* layout) { return layout->bit_width(); }},
                   member.type);
    m_members.push_back(std::move(member));
}

std::pair<std::vector<Value>, size_t> Layout::parse_bits(std::span<uint8_t> data, Endianess endianess) const {
    assert(endianess == Endianess::Big);
    std::vector<Value> values;

    BitsView view(data);
    std::size_t end = 0;
    for (const auto& member : m_members) {
        std::visit(utils::overload{[&](const Type& type) {
                                       auto bits = view.get_bits(end, end + type.bit_width);
                                       // FIXME Handle correctly.
                                       assert(bits.has_value());
                                       end += type.bit_width;
                                       values.push_back(Value{type, *bits});
                                   },
                                   [&](const Layout* layout) {
                                       auto [sub_values, sub_count] = layout->parse_bits(data.subspan(end), endianess);
                                       std::move(sub_values.begin(), sub_values.end(), std::back_inserter(values));
                                       end += layout->bit_width();
                                   }},
                   member.type);
    }

    return {std::move(values), end};
}
} // namespace eto
