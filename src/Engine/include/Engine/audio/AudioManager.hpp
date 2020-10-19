#pragma once

#include <memory>
#include <vector>
#include <string>

#include <AL/al.h>
#include <AL/alc.h>

#include "Sound.hpp"

namespace engine {

class AudioManager {
public:
    AudioManager();

    ~AudioManager();

    // Only supports WAV and AIFF
    auto getSound(const std::string &path) -> std::shared_ptr<Sound>;

private:
    auto genSoundBuffer(const std::string &path) -> ALuint;
    void garbageCollectCurrentSounds();

private:
    ALCdevice *m_device;
    ALCcontext *m_context;

    std::vector<std::shared_ptr<engine::Sound>> m_currentSounds;
};

} // namespace engine
