#pragma once

#include <concepts>

namespace engine {

namespace d2 {

template<std::floating_point T> // note : any type of floating points
struct HitboxT {

    T width;
    T height;

};

/**
 * The hitbox of a 2d object is centred on his d2::Position like so :
 *
 *
 * (pos.x - width / 2, pos.y + height / 2)               (pos.x + width / 2, pos.y + height / 2)
 *
 *
 *                                        (pos.x, pos.y)
 *
 *
 * (pos.x - width / 2, pos.y - height / 2)               (pos.x + width / 2, pos.y - height / 2)
 *
 */

using Hitbox = HitboxT<double>;

} // namespace d2

} // namespace engine
