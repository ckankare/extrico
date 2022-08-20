#include <fmt/core.h>
#include <iostream>

int main() {
    std::cout << fmt::format("{} {}!", "Hello", "World") << std::endl;
    int i;
    std::cin >> i;
    return 0;
}
