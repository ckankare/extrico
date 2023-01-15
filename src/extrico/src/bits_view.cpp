#include "bits_view.hpp"

namespace eto {

std::optional<std::uint64_t> BitsView::get_bits(std::size_t begin, std::size_t end) const {

    if (end > m_count || end <= begin || end - begin > 64) {
        return {};
    }

    uint64_t result = 0;

    // TODO Endianess.
    // TODO This is probably slow (if that would ever matter).
    for (std::size_t index = begin; index < end; ++index) {
        result = result << 1 | get_bit(index);
    }

    return result;
}

uint8_t BitsView::get_bit(std::size_t index) const {
    const uint8_t mask = 0x01;
    const auto byte_index = index / 8;
    const auto bit_index = index % 8;
    return (m_span[byte_index] >> (7 - bit_index)) & mask;
}

} // namespace eto
