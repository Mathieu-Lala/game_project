#pragma once

#include <concepts>

namespace engine {

namespace d2 {

template<std::floating_point T>
struct PositionT {

    T x;
    T y;

};

using Position = PositionT<double>;


template<std::floating_point T>
struct VelocityT {

    T x;
    T y;

};

using Velocity = VelocityT<double>;


template<std::floating_point T>
struct ScaleT {

    T x;
    T y;

};

using Scale = ScaleT<double>;

} // namespace d2

} // namespace engine
