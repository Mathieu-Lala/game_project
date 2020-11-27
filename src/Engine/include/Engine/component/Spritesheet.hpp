#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <fstream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <fmt/format.h>

#include "Engine/component/Position.hpp"
#include "Engine/component/Cooldown.hpp"

namespace engine {

struct Spritesheet {
    struct Animation {
        std::string file;
        std::uint16_t width;
        std::uint16_t height;

        std::vector<d2::PositionT<std::uint16_t>> frames;

        std::chrono::milliseconds cooldown;
    };

    std::unordered_map<std::string, Animation> animations;

    Cooldown cooldown;

    std::uint16_t current_frame{0};
    std::string current_animation{"default"};

    static auto from_json(const std::string_view file) -> Spritesheet;
};

// todo : move in .cpp

inline void to_json(nlohmann::json &j, const engine::Spritesheet::Animation &animation)
{
    // clang-format off
    j = nlohmann::json{{
        "file", animation.file,
        "width", animation.width,
        "height", animation.height,
        "cooldown", animation.cooldown.count(),
        "frames", animation.frames,
    }};
    // clang-format on
}

inline void from_json(const nlohmann::json &j, engine::Spritesheet::Animation &animation)
{
    animation.file = j.at("file");
    animation.width = j.at("width");
    animation.height = j.at("height");
    animation.cooldown = std::chrono::milliseconds{j.at("cooldown")};
    animation.frames = j.at("frames").get<std::decay_t<decltype(animation.frames)>>();
}

inline void to_json(nlohmann::json &j, const engine::Spritesheet &sprite)
{
    j = nlohmann::json{{"object", {"animations", sprite.animations}}};
}

inline void from_json(const nlohmann::json &j, engine::Spritesheet &sprite)
{
    using namespace std::chrono_literals;

    sprite.animations = j.at("object").at("animations").get<std::decay_t<decltype(sprite.animations)>>();
    sprite.cooldown.is_in_cooldown = false;
    sprite.cooldown.cooldown = 0ms;
    sprite.cooldown.remaining_cooldown = 0ms;
}

inline auto Spritesheet::from_json(const std::string_view file) -> Spritesheet
{
    std::ifstream f(file.data());
    const auto json = nlohmann::json::parse(f);
    return json.get<Spritesheet>();
}

} // namespace engine
