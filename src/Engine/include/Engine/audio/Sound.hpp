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

    // Range [0.5; 2]
    auto setSpeed(float pitch) -> void;
    // Range [0, +inf]
    auto setVolume(float volume) -> void;
    auto setLoop(bool loop) -> void;

    auto getStatus() const -> SoundStatus;
    auto getSpeed() const -> float;
    auto getVolume() const -> float;
    auto doesLoop() const -> bool;

private:
    ALuint m_buffer;

};
} // namespace engine