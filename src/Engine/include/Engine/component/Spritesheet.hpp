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
    std::string file;

    std::uint16_t width;
    std::uint16_t height;

    std::vector<d2::PositionT<std::uint16_t>> frames;

    Cooldown speed;
    std::uint16_t current_frame{0};

    static auto from_json(const std::string_view file) -> Spritesheet;

};

inline
void to_json(nlohmann::json &j, const engine::Spritesheet &sprite)
{
    j = nlohmann::json{{"object", {
        "file", sprite.file,
        "width", sprite.width,
        "height", sprite.height,
        "frames", sprite.frames,
        "speed", sprite.speed.cooldown.count()
    }}};
}

inline
void from_json(const nlohmann::json &j, engine::Spritesheet &sprite)
{
    using namespace std::chrono_literals;

    sprite.file = j.at("object").at("file");
    sprite.width = j.at("object").at("width");
    sprite.height = j.at("object").at("height");
    sprite.frames = j.at("object").at("frames").get<std::decay_t<decltype(sprite.frames)>>();
    std::uint64_t value = j.at("object").at("speed");
    sprite.speed.cooldown = std::chrono::milliseconds{value};
    sprite.speed.is_in_cooldown = false;
    sprite.speed.remaining_cooldown = 0ms;
}

inline
auto Spritesheet::from_json(const std::string_view file) -> Spritesheet
{
    std::ifstream f(file.data());
    const auto json = nlohmann::json::parse(f);
    return json.get<Spritesheet>();
}

} // namespace engine
