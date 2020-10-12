#pragma once

#include <concepts>

#include "Engine/component/Position.hpp"

namespace engine {

namespace d2 {

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

template<std::floating_point T> // note : any type of floating points
struct HitboxT {
    T width;
    T height;

    enum WithEdgeInHitbox {
        WITHOUT_EDGE = 0,
        WITH_EDGE = 0,
    };

    template<WithEdgeInHitbox v = WITHOUT_EDGE>
    [[nodiscard("use this return value")]]
    static constexpr auto overlapped(const HitboxT &self, const PositionT<T> &self_pos, const HitboxT &other, const PositionT<T> &other_pos) noexcept
        -> bool
    {
        const auto ax = self_pos.x - self.width / 2.0;
        const auto aw = self_pos.x + self.width / 2.0;
        const auto bx = other_pos.x - other.width / 2.0;
        const auto bw = other_pos.x + other.width / 2.0;

        const auto ay = self_pos.y - self.height / 2.0;
        const auto ah = self_pos.y + self.height / 2.0;
        const auto by = other_pos.y - other.height / 2.0;
        const auto bh = other_pos.y + other.height / 2.0;

        if constexpr(v == WITH_EDGE)
            return aw >= bx && ax <= bw && ah >= by && ay <= bh;
        else
            return aw > bx && ax < bw && ah > by && ay < bh;
    }
};

using Hitbox = HitboxT<double>;

} // namespace d2

} // namespace engine
