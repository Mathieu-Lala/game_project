#include "Engine/audio/Sound.hpp"
#include "Engine/audio/AlErrorHandling.hpp"

#include <spdlog/spdlog.h>

engine::Sound::Sound(ALuint soundBuffer)
{
    alCall(alGenSources(1, &m_buffer));

    alCall(alSourcef(m_buffer, AL_PITCH, 1));
    alCall(alSourcef(m_buffer, AL_GAIN, 1.0f));
    alCall(alSource3f(m_buffer, AL_POSITION, 0, 0, 0));
    alCall(alSource3f(m_buffer, AL_VELOCITY, 0, 0, 0));
    alCall(alSourcei(m_buffer, AL_LOOPING, AL_FALSE));
    alCall(alSourcei(m_buffer, AL_BUFFER, static_cast<ALint>(soundBuffer)));
}

engine::Sound::Sound() : m_alive(false) {}

engine::Sound::~Sound() { forceDestroy(); }

std::shared_ptr<engine::Sound> engine::Sound::GetEmptySound() { return std::shared_ptr<Sound>(new Sound); }

auto engine::Sound::play() -> Sound &
{
    if (m_alive) alCall(alSourcePlay(m_buffer));

    return *this;
}

auto engine::Sound::stop() -> Sound &
{
    if (m_alive) alCall(alSourceStop(m_buffer));

    return *this;
}

// Range [0.5; 2]
auto engine::Sound::setSpeed(float speed) -> Sound &
{
    if (m_alive) {
        speed = std::clamp(speed, 0.5f, 2.f);
        alCall(alSourcef(m_buffer, AL_PITCH, speed));
    }

    return *this;
}
// Range [0, +inf]
auto engine::Sound::setVolume(float volume) -> Sound &
{
    if (m_alive) {
        volume = std::clamp(volume, 0.f, 99999.f);
        alCall(alSourcef(m_buffer, AL_GAIN, volume));
    }

    return *this;
}

auto engine::Sound::setLoop(bool loop) -> Sound &
{
    if (m_alive) alCall(alSourcei(m_buffer, AL_LOOPING, loop ? AL_TRUE : AL_FALSE));
    return *this;
}

auto engine::Sound::getStatus() const -> SoundStatus
{
    if (!m_alive) return SoundStatus::STOPPED;

    ALint state;
    alCall(alGetSourcei(m_buffer, AL_SOURCE_STATE, &state));

    switch (state) {
    case AL_INITIAL: return SoundStatus::INITIAL;
    case AL_PLAYING: return SoundStatus::PLAYING;
    case AL_PAUSED: return SoundStatus::PAUSED;
    case AL_STOPPED: return SoundStatus::STOPPED;
    default: std::abort();
    }
}


auto engine::Sound::getSpeed() const -> float
{
    if (!m_alive) return 0;

    float result;
    alCall(alGetSourcef(m_buffer, AL_PITCH, &result));
    return result;
}

auto engine::Sound::getVolume() const -> float
{
    if (!m_alive) return 0;

    float result;
    alCall(alGetSourcef(m_buffer, AL_GAIN, &result));
    return result;
}
auto engine::Sound::doesLoop() const -> bool
{
    if (!m_alive) return false;

    ALint result;
    alCall(alGetSourcei(m_buffer, AL_LOOPING, &result));
    return result;
}

auto engine::Sound::forceDestroy() -> void
{
    if (!m_alive) return;

    m_alive = false;

    alSourceStop(m_buffer);
    alDeleteSources(1, &m_buffer);
}
