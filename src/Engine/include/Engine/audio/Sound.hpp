#pragma once

#include "AL/al.h"

namespace engine {

    enum class SoundStatus {
        INITIAL,
        PLAYING,
        PAUSED,
        STOPPED,
    };
class Sound {
public:
    Sound(ALuint soundBuffer);
    ~Sound();

    auto play() -> void;
    auto stop() -> void;

    auto getStatus() const -> SoundStatus;
private:
    ALuint m_buffer;

};
} // namespace engine