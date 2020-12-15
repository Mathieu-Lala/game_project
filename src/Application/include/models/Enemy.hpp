#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <utility>

#include <glm/vec4.hpp>
#include <nlohmann/json.hpp>

#include <Engine/component/Hitbox.hpp>
#include <Engine/component/Scale.hpp>

namespace game {

struct Enemy {

    std::string asset;

    float health;
    float speed;

    glm::vec4 color;

    engine::d2::HitboxSolid hitbox;

    engine::d2::Scale scale;

    std::vector<std::string> spells;

    bool is_boss;

    float view_range;
    float attack_range;

    std::uint32_t experience;

    std::vector<std::string> tag;

};

inline void to_json([[maybe_unused]] nlohmann::json &j, [[maybe_unused]] const Enemy &enemy)
{
    // clang-format off
    // j = nlohmann::json{{

    // }};
    // clang-format on
}

inline void from_json(const nlohmann::json &j, Enemy &enemy)
{

    enemy.asset = j.at("asset");
    enemy.health = j.at("health");
    enemy.speed = j.at("speed");
    enemy.color.r = j.at("color").at("r");
    enemy.color.g = j.at("color").at("g");
    enemy.color.b = j.at("color").at("b");
    enemy.color.a = j.at("color").value("a", 1.0f);
    enemy.hitbox.width = j.at("hitbox").at("x");
    enemy.hitbox.height = j.at("hitbox").at("y");
    enemy.scale.x = j.at("scale").at("x");
    enemy.scale.y = j.at("scale").at("y");
    enemy.spells = j.at("spells").get<std::vector<std::string>>();
    enemy.is_boss = j.value("is_boss", false);
    enemy.view_range = j.at("view_range");
    enemy.attack_range = j.at("attack_range");
    enemy.experience = j.at("experience");
    enemy.tag = j.value("tag", std::vector<std::string>{});
}

struct EnemyDatabase {
    std::unordered_map<std::string, Enemy> db;

    auto fromFile(const std::string_view path) -> bool;
};

} // namespace game
