#pragma once

#include <concepts>

namespace engine {

namespace d2 {

template<std::floating_point T>
struct AccelerationT {
    T x;
    T y;
};

using Acceleration = AccelerationT<double>;

} // namespace d2

} // namespace engine
