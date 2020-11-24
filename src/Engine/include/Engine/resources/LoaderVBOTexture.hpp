#pragma once

#include <memory>

#include <entt/entt.hpp>

namespace engine {

struct VBOTexture;

struct LoaderVBOTexture : entt::resource_loader<LoaderVBOTexture, VBOTexture> {
    template<typename... Args>
    auto load(Args &&... args) const -> std::shared_ptr<VBOTexture>
    {
        return std::shared_ptr<VBOTexture>(new VBOTexture{VBOTexture::ctor(args...)}, VBOTexture::dtor);
    }
};

using CacheVBOTexture = entt::resource_cache<VBOTexture>;

} // namespace engine
