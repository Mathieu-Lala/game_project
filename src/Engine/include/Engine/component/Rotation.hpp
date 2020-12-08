#pragma once

#include <concepts>

namespace engine {

namespace d2 {

template<std::floating_point T>
struct RotationT {
    // Radian
    T angle;
};

using Rotation = RotationT<double>;

} // namespace d2

} // namespace engine
