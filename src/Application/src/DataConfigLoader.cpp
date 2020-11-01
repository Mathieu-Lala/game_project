#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>

#include <Engine/helpers/DrawableFactory.hpp>
#include <Engine/Graphics/Shader.hpp>
#include <Engine/Event/Event.hpp>
#include <Engine/Settings.hpp>
#include <Engine/audio/AudioManager.hpp>
#include <Engine/Core.hpp>

#include "classes/ClassFactory.hpp"
#include "DataConfigLoader.hpp"
#include "component/all.hpp"
#include "factory/EntityFactory.hpp"

using namespace std::chrono_literals; // ms ..

auto game::DataConfigLoader::loadPlayerConfigFile(const std::string_view filename, entt::registry &world, entt::entity &player)
    -> entt::entity
{
    static auto holder = engine::Core::Holder{};

    spdlog::info("Create a new Player from the file: " + std::string(filename.data()));

    std::ifstream file(filename.data());

    if (!file.is_open()) { spdlog::error("Can't open the given file"); }
    nlohmann::json data = nlohmann::json::parse(file);

    world.emplace<entt::tag<"player"_hs>>(player);
    world.emplace<engine::d3::Position>(player, 0.0, 0.0, EntityFactory::get_z_layer<EntityFactory::LAYER_PLAYER>());
    world.emplace<engine::d2::Velocity>(player, 0.0, 0.0);
    world.emplace<engine::d2::Acceleration>(player, 0.0, 0.0);
    world.emplace<engine::d2::Scale>(player, 1.0, 1.0);
    world.emplace<engine::d2::HitboxSolid>(player, 1.0, 1.0);
    world.emplace<engine::Drawable>(player, engine::DrawableFactory::rectangle());
    engine::DrawableFactory::fix_color(world, player, {0, 0, 1});
    engine::DrawableFactory::fix_texture(world, player, holder.instance->settings().data_folder + "textures/player.jpeg");
    world.emplace<Health>(player, float(data["stats"]["hp"]), float(data["stats"]["hp"]));
    world.emplace<AttackDamage>(player, data["stats"]["atk"]);
    world.emplace<Level>(player, 0u, 0u, 10u);
    world.emplace<KeyPicker>(player);
    world.emplace<SpellSlots>(player);

    return player;
}

auto game::DataConfigLoader::loadClassConfigFile(
    const std::string_view filename, [[maybe_unused]] entt::registry &world, [[maybe_unused]] entt::entity &player, Classes cl)
    -> void
{
    spdlog::info("Create a new Class from the file: " + std::string(filename.data()));

    std::ifstream file(filename.data());

    if (!file.is_open()) { spdlog::error("Can't open the given file"); }
    nlohmann::json data = nlohmann::json::parse(file);

    nlohmann::json classRoot = [cl, &data]() {
        switch (cl) {
        case Classes::FARMER: return data["farmer"];
        case Classes::SHOOTER: return data["shooter"];
        case Classes::SOLDIER: return data["soldier"];
        case Classes::SORCERER: return data["sorcerer"];
        default: std::abort();
        }
    }();

    world.emplace<ClassFactory>(
        player, cl, classRoot["desc"], classRoot["cooldown"], classRoot["range"], classRoot["damage"], classRoot["isRanged"]);

    for (int i = 0; auto &spellId : classRoot["spellsId"])
        world.get<SpellSlots>(player).spells[i++] = Spell::create(static_cast<SpellFactory::ID>(spellId.get<int>()));

    spdlog::info("{} class successfully created", classRoot["name"]);
}

void game::DataConfigLoader::reloadFiles() { std::cout << "Reload all files" << std::endl; }
