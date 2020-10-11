#pragma once

#include <entt/entt.hpp>

#include <Engine/component/Acceleration.hpp>

namespace engine {
    class Movement
    {
    public:
        void moveAxis([[maybe_unused]] entt::registry &world, entt::entity &player, const engine::d2::Acceleration &accel)
        { 
            world.get<engine::d2::Acceleration>(player) = accel;

            return;
        };
    };
}
