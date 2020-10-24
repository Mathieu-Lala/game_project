#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>

#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Graphics/Shader.hpp>

#include <classes/ClassFactory.hpp>
#include <Declaration.hpp>
#include <DataConfigLoader.hpp>
#include <EntityDepth.hpp>
#include <component/all.hpp>

using namespace std::chrono_literals; // ms ..

auto engine::DataConfigLoader::loadPlayerConfigFile(const std::string_view &filename, entt::registry &world, entt::entity &player) -> entt::entity
{
    spdlog::info("Create a new Player from the file: " + std::string(filename.data()));

    std::ifstream file(filename.data());

    if (!file.is_open()) {
        spdlog::error("Can't open the given file");
    }
    nlohmann::json data = nlohmann::json::parse(file);

    world.emplace<entt::tag<"player"_hs>>(player);
    world.emplace<engine::d3::Position>(player, 0.0, 0.0, Z_COMPONENT_OF(game::EntityDepth::PLAYER));
    world.emplace<engine::d2::Velocity>(player, 0.0, 0.0);
    world.emplace<engine::d2::Acceleration>(player, 0.0, 0.0);
    world.emplace<engine::d2::Scale>(player, 1.0, 1.0);
    world.emplace<engine::d2::HitboxSolid>(player, 1.0, 1.0);
    world.emplace<engine::Drawable>(player, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, player, {0, 0, 1});
    engine::DrawableFactory::fix_texture(world, player, DATA_DIR "textures/player.jpeg");
    world.emplace<game::Health>(player, float(data["stats"]["hp"]), float(data["stats"]["hp"]));
    world.emplace<game::AttackCooldown>(player, false, static_cast<std::chrono::milliseconds>(data["stats"]["cooldown"]), 0ms);
    world.emplace<game::AttackDamage>(player, data["stats"]["atk"]);
    world.emplace<game::Level>(player, 0u, 0u, 10u);

    file.close();
    return player;
}

auto engine::DataConfigLoader::loadClassConfigFile(const std::string_view &filename,
    [[maybe_unused]] entt::registry &world,
    [[maybe_unused]] entt::entity &player,
    engine::Classes cl) -> entt::entity
{
    spdlog::info("Create a new Class from the file: " + std::string(filename.data()));

    std::ifstream file(filename.data());

    if (!file.is_open()) {
        spdlog::error("Can't open the given file");
    }
    nlohmann::json data = nlohmann::json::parse(file);

    switch(cl) {
        case engine::Classes::FARMER:
            world.emplace<engine::ClassFactory>(player, engine::Classes::FARMER, data["farmer"]["desc"], data["farmer"]["cooldown"], data["farmer"]["range"], data["farmer"]["damage"], data["farmer"]["isRanged"]);
            spdlog::info("Farmer class successfully created");
            break;
        case engine::Classes::SHOOTER:
            world.emplace<engine::ClassFactory>(player, engine::Classes::SHOOTER, data["shooter"]["desc"], data["shooter"]["cooldown"], data["shooter"]["range"], data["shooter"]["damage"], data["shooter"]["isRanged"]);
            spdlog::info("Shooter class successfully created");
            break;
        case engine::Classes::SOLDIER:
            world.emplace<engine::ClassFactory>(player, engine::Classes::SOLDIER, data["soldier"]["desc"], data["soldier"]["cooldown"], data["soldier"]["range"], data["soldier"]["damage"], data["soldier"]["isRanged"]);
            spdlog::info("Soldier class successfully created");
            break;
        case engine::Classes::SORCERER:
            world.emplace<engine::ClassFactory>(player, engine::Classes::SORCERER, data["sorcerer"]["desc"], data["sorcerer"]["cooldown"], data["sorcerer"]["range"], data["sorcerer"]["damage"], data["sorcerer"]["isRanged"]);
            spdlog::info("Sorcerer class successfully created");
            break;
    }

    file.close();
    return player;
}

void engine::DataConfigLoader::reloadFiles() { std::cout << "Reload all files" << std::endl; }