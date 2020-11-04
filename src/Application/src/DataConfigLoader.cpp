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

auto game::DataConfigLoader::loadClassDatabase(const std::string_view path) -> ClassDatabase
{
    spdlog::info("Loading class database file: '{}'", path.data());

    std::ifstream file(path.data());

    if (!file.is_open()) { spdlog::error("Can't open the given file"); }
    nlohmann::json classes = nlohmann::json::parse(file);


    ClassDatabase database;

    for (Class::ID id = 0; const auto &[name, data] : classes.items()) {
        std::vector<SpellFactory::ID> spells;
        spdlog::info("debug : {} : {}\n---", name, data.dump());

        for (const auto &spell : data["spells"])
                spells.push_back(static_cast<SpellFactory::ID>(spell.get<int>()));

        database[id] = Class{
            .id = id,
            .name = name,
            .description = data["desc"].get<std::string>(),
            .spells = spells,

            .maxHealth = data["maxHealth"].get<float>(),
            .damage = data["damage"].get<float>(),
        };

        id++;
    }

    return database;
}

void game::DataConfigLoader::reloadFiles() { std::cout << "Reload all files" << std::endl; }
