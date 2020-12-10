#pragma once

#include <fmt/format.h>
#include <entt/entt.hpp>

#include "Engine/resources/LoaderTexture.hpp"
#include "Engine/Core.hpp"

namespace engine::helper {

inline std::uint32_t loadTexture(const std::string &path, bool mirrored_repeated = false)
{
    static engine::Core::Holder holder{};

    auto key = entt::hashed_string{fmt::format("resource/texture/identifier/{}_{}", path, mirrored_repeated).data()};
    const auto &resource =
        holder.instance->getCache<engine::Texture>().load<engine::LoaderTexture>(key, path, mirrored_repeated);

    return resource->id;
}

} // namespace engine::helper
