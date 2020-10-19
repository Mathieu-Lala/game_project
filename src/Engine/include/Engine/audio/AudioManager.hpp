#pragma once

#include <memory>
#include <vector>
#include <string>

#include <AL/al.h>
#include <AL/alc.h>

#include "Sound.hpp"
#include "Engine/resources/AudioFileLoader.hpp"

namespace engine {

class AudioManager {
public:
    AudioManager();

    ~AudioManager();

    // Only supports WAV and AIFF
    auto getSound(const std::string &path) -> std::shared_ptr<Sound>;

private:
    void garbageCollectCurrentSounds();

private:
    ALCdevice *m_device;
    ALCcontext *m_context;

    std::vector<std::shared_ptr<engine::Sound>> m_currentSounds;
    AudioFileCache m_audioFileCache;
};

} // namespace engine
