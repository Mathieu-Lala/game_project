#pragma once

#include <concepts>

namespace engine {

namespace d2 {

template<std::floating_point T> // note : any type of floating points
struct PositionT {

    T x;
    T y;
    T z;

};

using Position = PositionT<double>;

} // namespace d2

} // namespace engine
