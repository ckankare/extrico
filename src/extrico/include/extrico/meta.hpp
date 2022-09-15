#pragma once

#include <algorithm>
#include <array>
#include <memory>

namespace utils {

template <typename... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};
template <typename... Ts>
overload(Ts...) -> overload<Ts...>;

template <typename T, typename... Ts>
struct are_same : std::conjunction<std::is_same<T, Ts>...> {};

template <typename... Ts>
constexpr bool are_same_v = are_same<Ts...>::value;

template <typename T, typename... Args>
constexpr auto make_array(T&& arg, Args&&... args) {
    static_assert(are_same_v<T, Args...>, "All elements must be of same type");
    return std::array<std::decay_t<T>, sizeof...(args) + 1>{{std::forward<T>(arg), std::forward<Args>(args)...}};
}

template <typename T1, typename T2>
constexpr T1 unique_cast(std::unique_ptr<T2>& ptr) {
    return dynamic_cast<T1>(ptr.get());
}

} // namespace utils
