#pragma once

#include <memory>
#include <string>

#include <entt/entt.hpp>

#include "Engine/audio/AudioFileBuffer.hpp"

namespace engine {

struct AudioFileLoader : entt::resource_loader<AudioFileLoader, AudioFileBuffer> {
    auto load(const std::string &path) const -> std::shared_ptr<AudioFileBuffer>;
};

using AudioFileCache = entt::resource_cache<AudioFileBuffer>;

} // namespace engine
