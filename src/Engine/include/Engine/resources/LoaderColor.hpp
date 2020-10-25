#pragma once

#include <memory>

#include <entt/entt.hpp>

#include "Engine/component/Color.hpp" // note : this include is optional in this context

namespace engine {

struct Color;

struct LoaderColor : entt::resource_loader<LoaderColor, Color> {
    template<typename... Args>
    auto load(Args &&... args) const -> std::shared_ptr<Color>
    {
        return std::shared_ptr<Color>(new Color{Color::ctor(std::move(args...))}, Color::dtor);
    }
};

using CacheColor = entt::resource_cache<Color>;

} // namespace engine
