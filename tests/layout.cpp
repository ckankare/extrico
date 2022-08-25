#include <catch2/catch_test_macros.hpp>

#include <array>

#include <extrico/layout.hpp>

TEST_CASE("Layout parse_bits(), simple layout") {
    std::array<uint8_t, 3> data{0x5E, 0xB6, 0x9D};
    // 0101'1110'1011'0110'1001'1101
    eto::Layout layout("s");
    layout.add_member(eto::Member{"foo", eto::Type{eto::BaseType::Unsigned, 3}});
    layout.add_member(eto::Member{"bar", eto::Type{eto::BaseType::Unsigned, 11}});
    layout.add_member(eto::Member{"baz", eto::Type{eto::BaseType::Unsigned, 4}});

    auto [values, end] = layout.parse_bits(data, eto::Endianess::Big);

    REQUIRE(values.size() == 3);
    CHECK(end == 18);

    CHECK(values[0].bits == 0b010);
    CHECK(values[1].bits == 0b1'1110'1011'01);
    CHECK(values[2].bits == 0b10'10);
}
