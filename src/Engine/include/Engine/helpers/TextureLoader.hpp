#pragma once

#include <fmt/format.h>
#include <entt/entt.hpp>

#include "Engine/resources/LoaderTexture.hpp"
#include "Engine/Core.hpp"
#include "Engine/api/Core.hpp"

namespace engine::helper {

inline std::uint32_t loadTexture(const std::string &path)
{
    auto key = entt::hashed_string{fmt::format("resource/texture/identifier/{}", path).data()};
    const auto &resource = engine::api::getCore()->getCache<engine::Texture>().load<engine::LoaderTexture>(key, path);

    return resource->id;
}

} // namespace engine::helper
