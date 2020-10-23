#pragma once

#include <concepts>
#include <cmath>

namespace engine {

namespace d3 {

template<std::floating_point T>
struct PositionT {
    T x;
    T y;
    T z;
};

using Position = PositionT<double>;

template<std::floating_point T>
[[nodiscard]] constexpr auto distanceSquared(const PositionT<T> &a, const PositionT<T> &b) noexcept -> double
{
    T x = a.x - b.x;
    T y = a.y - b.y;

    return x * x + y * y;
}

template<std::floating_point T>
[[nodiscard]] constexpr auto distance(const PositionT<T> &a, const PositionT<T> &b) noexcept -> double
{
    return std::sqrt(distanceSquared(a, b));
}

} // namespace d3

} // namespace engine
