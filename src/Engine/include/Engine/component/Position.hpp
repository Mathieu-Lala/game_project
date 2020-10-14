#pragma once

#include <concepts>

namespace engine {

namespace d3 {

template<std::floating_point T>
struct PositionT {
    T x;
    T y;
    T z;
};

using Position = PositionT<double>;

} // namespace d3

} // namespace engine
