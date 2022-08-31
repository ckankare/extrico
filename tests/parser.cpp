#include <catch2/catch_test_macros.hpp>

#include <array>

#include <extrico/parser.hpp>

using namespace std::literals;

TEST_CASE("Parser, simple layout") {
    auto source = R"(layout foo {
           bar: u32;
           baz: i8;
           foo_bar: b10;
        };)"sv;

    eto::Parser parser(source);

    auto layout = parser.parse();
    REQUIRE(layout != nullptr);
    CHECK(layout->name() == "foo");
    CHECK(layout->bit_width() == 32 + 8 + 10);
    REQUIRE(layout->size() == 3);
    CHECK((*layout)[0].name == "bar");
    CHECK((*layout)[1].name == "baz");
    CHECK((*layout)[2].name == "foo_bar");
}
