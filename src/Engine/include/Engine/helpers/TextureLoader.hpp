#pragma once

#include <fmt/format.h>
#include <entt/entt.hpp>

#include <spdlog/spdlog.h>

#include "Engine/resources/LoaderTexture.hpp"
#include "Engine/Core.hpp"

namespace engine::helper {

inline std::uint32_t loadTexture(const std::string &path, bool mirrored_repeated = false)
{
    static engine::Core::Holder holder{};

    auto key = entt::hashed_string{fmt::format("resource/texture/identifier/{}_{}", path, mirrored_repeated).data()};
    if (const auto &resource =
            holder.instance->getCache<engine::Texture>().load<engine::LoaderTexture>(key, path, mirrored_repeated);
        resource) {
        return resource->id;
    } else {
        spdlog::error("Could not load {}: ", path);
        return 0;
    }
}

} // namespace engine::helper
