#include "layout.hpp"

#include <cassert>
#include <iterator>

#include <fmt/core.h>

#include "bits_view.hpp"
#include "meta.hpp"

namespace eto {

namespace {
    template <typename T>
    T reinterpret_bits(uint64_t value) {
        static_assert(sizeof(T) <= sizeof(uint64_t));
        T dest;
        std::memcpy(&dest, &value, sizeof(dest));
        return dest;
    }

    int64_t sign_extend(uint64_t value, uint8_t high_bit) {
        assert(high_bit > 0);
        if (high_bit < 63 && (value >> high_bit) == 1) {
            // This is always well-defined.
            return (static_cast<int64_t>(value) << (63 - high_bit)) >> (63 - high_bit);
        }

        return reinterpret_bits<int64_t>(value);
    }

    std::string bits_to_string(uint64_t bits, Type type) {
        const auto nb_hex_numbers = (type.bit_width + 7) / 8;
        switch (type.base) {
            case BaseType::Boolean:
                return fmt::format(fmt::runtime("{} (0b{:0" + std::to_string(type.bit_width) + "b})"), bits > 0 ? 1 : 0,
                                   bits);
            case BaseType::Unsigned:
                return fmt::format(fmt::runtime("{} (0x{:0" + std::to_string(nb_hex_numbers) + "x})"), bits, bits);
            case BaseType::Signed:
                return fmt::format(fmt::runtime("{} (0x{:0" + std::to_string(nb_hex_numbers) + "x})"),
                                   sign_extend(bits, type.bit_width - 1), bits);
            case BaseType::Float:
                switch (type.bit_width) {
                    case 32:
                        return fmt::format(fmt::runtime("{} (0x{:0" + std::to_string(nb_hex_numbers) + "x})"),
                                           reinterpret_bits<float>(bits), bits);
                    case 64:
                        return fmt::format(fmt::runtime("{} (0x{:0" + std::to_string(nb_hex_numbers) + "x})"),
                                           reinterpret_bits<double>(bits), bits);
                    default: assert(false);
                }
        };

        // TODO Unreachable
        return fmt::format(fmt::runtime("{} (0x{:0" + std::to_string(nb_hex_numbers) + "x})"), "<unknown>", bits);
    }
} // namespace

void Layout::add_member(Member member) noexcept {
    m_total_bit_width +=
        std::visit(utils::overload{[&](const Type& type) { return static_cast<uint64_t>(type.bit_width); },
                                   [&](const Layout* layout) { return layout->bit_width(); }},
                   member.type);
    m_members.push_back(std::move(member));
}

std::string Layout::to_string(std::span<Value> values) const noexcept {
    std::string result = m_name + " = {\n";

    auto member = m_members.begin();

    for (auto value = values.begin(); value != values.end(); ++value) {
        result += "  " + member->name + " = ";
        std::visit(utils::overload{[&](const Type& type) { result += bits_to_string(value->bits, value->type) + '\n'; },
                                   [&](const Layout* layout) {
                                       assert(false); // TODO Implement.
                                       auto sub_string = layout->to_string({value, values.end()});
                                       result += '\n';
                                   }},
                   member->type);
        ++member;
    }

    result += '}';
    return result;
}

std::pair<std::vector<Value>, size_t> Layout::parse_bits(std::span<uint8_t> data, Endianess endianess) const noexcept {
    assert(endianess == Endianess::Big);
    std::vector<Value> values;

    BitsView view(data);
    std::size_t end = 0;
    bool exit = false;
    for (const auto& member : m_members) {
        std::visit(utils::overload{[&](const Type& type) {
                                       auto bits = view.get_bits(end, end + type.bit_width);
                                       if (!bits) {
                                           // TODO Try to propagate the error somehow, so it is
                                           // then visible or something.
                                           exit = true;
                                           return;
                                       }

                                       end += type.bit_width;
                                       values.push_back(Value{type, *bits});
                                   },
                                   [&](const Layout* layout) {
                                       auto [sub_values, sub_count] = layout->parse_bits(data.subspan(end), endianess);
                                       std::move(sub_values.begin(), sub_values.end(), std::back_inserter(values));
                                       end += layout->bit_width();
                                   }},
                   member.type);
        if (exit || end >= data.size() * 8) {
            break;
        }
    }

    return {std::move(values), end};
}
} // namespace eto
