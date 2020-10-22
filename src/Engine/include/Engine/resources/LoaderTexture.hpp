#pragma once

#include <memory>

#include <entt/entt.hpp>

namespace engine {

struct Texture;

struct LoaderTexture : entt::resource_loader<LoaderTexture, Texture> {
    auto load(const std::string_view path, std::array<float, 4ul> &&) const -> std::shared_ptr<Texture>;
};

using CacheTexture = entt::resource_cache<Texture>;

} // namespace engine
