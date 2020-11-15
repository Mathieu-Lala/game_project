#pragma once

#include <memory>

#include <entt/entt.hpp>

namespace engine {

struct Texture;

struct LoaderTexture : entt::resource_loader<LoaderTexture, Texture> {
    template<typename... Args>
    auto load(Args &&... args) const -> std::shared_ptr<Texture>
    {
        return std::shared_ptr<Texture>(new Texture{Texture::ctor(args...)}, Texture::dtor);
    }
};

using CacheTexture = entt::resource_cache<Texture>;

} // namespace engine
