#pragma once

#include <cassert>
#include <cstdint>
#include <numeric>
#include <optional>
#include <span>

namespace eto {

class BitsView {
public:
    BitsView(std::span<uint8_t> span, std::size_t count) : m_span(span), m_count(count) {}
    BitsView(std::span<uint8_t> span) : BitsView(span, span.size() * 8) {
        assert(span.size() <= std::numeric_limits<std::size_t>::max() / 8);
    }

    std::optional<uint64_t> get_bits(std::size_t begin, std::size_t end) const;

private:
    uint8_t get_bit(std::size_t index) const;
    std::span<uint8_t> m_span;
    std::size_t m_count;
};
} // namespace eto
