#include <cmath>
#include <numbers>

#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Core.hpp>

#include "component/all.hpp"
#include "factory/ParticuleFactory.hpp"
#include "factory/EntityFactory.hpp"

template<>
auto game::ParticuleFactory::create<game::Particule::ID::HITMARKER>(entt::registry &world, const glm::vec2 &pos) -> void
{
    constexpr auto particule_count = 10.0f;
    constexpr auto speed = 2.0f;

    for (float i = 0; i != particule_count; i++) {
        const auto angle = i * 2 * std::numbers::pi_v<float> / particule_count;
        const auto particule_pos = pos + glm::vec2{std::cos(angle), std::sin(angle)};

        auto e = world.create();
        // note : i think there is a probleme here !!!
        world.emplace<engine::d3::Position>(e, particule_pos.x, particule_pos.y, 1);
            //EntityFactory::get_z_layer<EntityFactory::LAYER_PLAYER>());
        world.emplace<engine::d2::Scale>(e, 0.1, 0.1);
        world.emplace<engine::d2::Velocity>(e, (particule_pos.x - pos.x) * speed, (particule_pos.y - pos.y) * speed);
        world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
        world.emplace<engine::Lifetime>(e, 600ms);
        world.emplace<Particule>(e, Particule::HITMARKER);
        engine::DrawableFactory::fix_color(world, e, {255.0f / 255.0f, 92.0f / 255.0f, 103.0f / 255.0f});
    }
}
