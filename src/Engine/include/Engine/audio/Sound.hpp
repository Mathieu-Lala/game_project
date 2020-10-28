#pragma once

#include <memory>
#include <AL/al.h>

namespace engine {

enum class SoundStatus {
    INITIAL,
    PLAYING,
    PAUSED,
    STOPPED,
};

class Sound {
public:
    explicit Sound(ALuint soundBuffer);
    ~Sound();

    static std::shared_ptr<Sound> GetEmptySound();

    auto play() -> Sound &;
    auto stop() -> Sound &;

    // Range [0.5; 2]
    auto setSpeed(float pitch) -> Sound &;
    // Range [0, +inf]
    auto setVolume(float volume) -> Sound &;
    auto setLoop(bool loop) -> Sound &;

    auto getStatus() const -> SoundStatus;
    auto getSpeed() const -> float;
    auto getVolume() const -> float;
    auto doesLoop() const -> bool;

    // Don't call that yourself
    auto forceDestroy() -> void;

private:
    Sound(); // Not alive;

private:
    ALuint m_buffer;
    bool m_alive = true;
};

} // namespace engine
