#pragma once

#include <memory>

#include <entt/entt.hpp>

#include "Engine/audio/AudioFileBuffer.hpp"

namespace engine {

struct AudioFileLoader : entt::resource_loader<AudioFileLoader, AudioFileBuffer> {
    template<typename... Args>
    auto load(Args &&... args) const -> std::shared_ptr<AudioFileBuffer>
    {
        return std::make_shared<AudioFileBuffer>(std::move(args)...);
    }
};

using AudioFileCache = entt::resource_cache<AudioFileBuffer>;

} // namespace engine
