#pragma once

#include <concepts>

namespace engine {

namespace d2 {

template<std::floating_point T> // note : any type of floating points
struct ScaleT {

    T x;
    T y;

};

using Scale = ScaleT<double>;

} // namespace d2

} // namespace engine
