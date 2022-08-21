#include <catch2/catch_test_macros.hpp>

#include <array>

#include <extrico/bits_view.hpp>

TEST_CASE("BitsView gets_bits()") {
    std::array<uint8_t, 3> data{0x5E, 0xB6, 0x9D};
    // 0101'1110'1011'0110'1001'1101
    eto::BitsView view(data);

    CHECK(*view.get_bits(0, 3) == 0b010);
    CHECK(*view.get_bits(1, 3) == 0b10);

    CHECK(*view.get_bits(8, 9) == 0b1);
    CHECK(*view.get_bits(8, 17) == 0b1011'0110'1);

    CHECK(*view.get_bits(22, 24) == 0b01);

    CHECK(*view.get_bits(2, 21) == 0b01'1110'1011'0110'1001'1);
}
