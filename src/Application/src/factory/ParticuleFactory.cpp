#include <cmath>
#include <numbers>
#include <chrono>

#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Core.hpp>

#include "models/Spell.hpp"

#include "component/all.hpp"
#include "factory/ParticuleFactory.hpp"
#include "factory/EntityFactory.hpp"
#include <Engine/component/Rotation.hpp>

using namespace std::chrono_literals;

template<>
auto game::ParticuleFactory::create<game::Particule::ID::HITMARKER>(
    entt::registry &world, const glm::vec2 &pos, const glm::vec3 &color) -> void
{
    constexpr auto particule_count = 10.0f;
    constexpr auto speed = 2.0f;

    for (float i = 0; i != particule_count; i++) {
        const auto angle = i * 2 * std::numbers::pi_v<float> / particule_count;
        const auto particule_pos = pos + glm::vec2{std::cos(angle), std::sin(angle)};

        auto e = world.create();
        world.emplace<engine::d3::Position>(
            e, particule_pos.x, particule_pos.y, EntityFactory::get_z_layer<EntityFactory::LAYER_PLAYER>());
        world.emplace<engine::d2::Scale>(e, 0.1, 0.1);
        world.emplace<engine::d2::Rotation>(e, 0.f);
        world.emplace<engine::d2::Velocity>(e, (particule_pos.x - pos.x) * speed, (particule_pos.y - pos.y) * speed);
        world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
        world.emplace<engine::Lifetime>(e, 300ms);
        world.emplace<Particule>(e, Particule::HITMARKER);
        engine::DrawableFactory::fix_color(world, e, {color.x / 255.0f, color.y / 255.0f, color.z / 255.0f, 1.0f});
    }
}

template<>
auto game::ParticuleFactory::create<game::Particule::ID::POSITIVE>(
    entt::registry &world, const glm::vec2 &pos, const glm::vec3 &color) -> void
{
    constexpr auto particule_count = 10.0f;
    constexpr auto speed = 2.0f;


    for (float i = 0; i != particule_count; i++) {
        const auto angle = i * 2 * std::numbers::pi_v<float> / particule_count;
        const auto particule_pos = pos + glm::vec2{std::cos(angle), std::sin(angle)};

        auto e = world.create();
        world.emplace<engine::d3::Position>(
            e, particule_pos.x, particule_pos.y, EntityFactory::get_z_layer<EntityFactory::LAYER_PLAYER>());
        world.emplace<engine::d2::Scale>(e, 0.1, 0.1);
        world.emplace<engine::d2::Rotation>(e, 0.f);
        world.emplace<engine::d2::Velocity>(e, -(particule_pos.x - pos.x) * speed, -(particule_pos.y - pos.y) * speed);
        world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
        world.emplace<engine::Lifetime>(e, 300ms);
        world.emplace<Particule>(e, Particule::HITMARKER);
        engine::DrawableFactory::fix_color(world, e, {color.x / 255.0f, color.y / 255.0f, color.z / 255.0f, 1.0f});
    }
}

template<>
auto game::ParticuleFactory::create<game::Particule::ID::NEUTRAL>(
    entt::registry &world, const glm::vec2 &pos, const glm::vec3 &color) -> void
{
    constexpr auto particule_count = 10.0f;
    constexpr auto speed = 2.0f;
    auto old_angle = 9 * 2 * std::numbers::pi_v<float> / particule_count;
    auto old_value = pos + glm::vec2{std::cos(old_angle), std::sin(old_angle)};

    for (float i = 0; i != particule_count; i++) {
        const auto angle = i * 2 * std::numbers::pi_v<float> / particule_count;
        const auto particule_pos = pos + glm::vec2{std::cos(angle), std::sin(angle)};
        auto e = world.create();
        world.emplace<engine::d3::Position>(
            e, particule_pos.x, particule_pos.y, EntityFactory::get_z_layer<EntityFactory::LAYER_PLAYER>());
        world.emplace<engine::d2::Scale>(e, 0.1, 0.1);
        world.emplace<engine::d2::Rotation>(e, 0.f);
        world.emplace<engine::d2::Velocity>(
            e, (old_value.x - particule_pos.x) * speed, (old_value.y - particule_pos.y) * speed);
        world.emplace<engine::Drawable>(e, engine::DrawableFactory::rectangle());
        world.emplace<engine::Lifetime>(e, 600ms);
        world.emplace<Particule>(e, Particule::HITMARKER);
        engine::DrawableFactory::fix_color(world, e, {color.x / 255.0f, color.y / 255.0f, color.z / 255.0f, 1.0f});
        old_value = particule_pos;
    }
}
