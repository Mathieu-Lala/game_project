#pragma once

#include <memory>

#include <entt/entt.hpp>

namespace engine {

struct Color;

struct LoaderColor : entt::resource_loader<LoaderColor, Color> {
    auto load(glm::vec3 &&color) const -> std::shared_ptr<Color>;
};

using CacheColor = entt::resource_cache<Color>;

} // namespace engine
