#pragma once

#include <concepts>

#include "Engine/component/Position.hpp"

namespace engine {

namespace d2 {

/**
 * The hitbox of a 2d object is centred on his d3::Position like so :
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

enum HitboxType {
    FLOATING, // entity can be traversed (cloud)
    SOLID  // entity can not be traversed (wall)
};

template<std::floating_point T, HitboxType Type>
struct HitboxT {
    T width;
    T height;
};

enum WithEdgeInHitbox {
    WITHOUT_EDGE = 0,
    WITH_EDGE = 1,
};

template<WithEdgeInHitbox Edge, std::floating_point T, HitboxType Self, HitboxType Other>
[[nodiscard]] constexpr
auto overlapped(const HitboxT<T, Self> &self, const d3::PositionT<T> &self_pos, const HitboxT<T, Other> &other, const d3::PositionT<T> &other_pos) noexcept
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

        if constexpr (Edge == WITH_EDGE)
            return aw >= bx && ax <= bw && ah >= by && ay <= bh;
        else
            return aw > bx && ax < bw && ah > by && ay < bh;
    }


template<HitboxType T = SOLID>
using Hitbox = HitboxT<double, T>;

using HitboxFloat = Hitbox<FLOATING>;
using HitboxSolid = Hitbox<SOLID>;

} // namespace d2

} // namespace engine
