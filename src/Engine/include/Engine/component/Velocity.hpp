#pragma once

#include <concepts>

namespace engine {

namespace d2 {

template<std::floating_point T>
struct VelocityT {
    using type = T;

    T x;
    T y;
};

using Velocity = VelocityT<double>;

} // namespace d2

} // namespace engine
