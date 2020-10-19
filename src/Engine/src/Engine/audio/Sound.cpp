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

engine::Sound::~Sound()
{
    alSourceStop(m_buffer);
    alDeleteSources(1, &m_buffer);
}

void engine::Sound::play()
{
    alCall(alSourcePlay(m_buffer));
}

void engine::Sound::stop() { alCall(alSourceStop(m_buffer)); }

// Range [0.5; 2]
auto engine::Sound::setSpeed(float pitch) -> void
{
    pitch = std::clamp(pitch, 0.5f, 2.f);
    alCall(alSourcef(m_buffer, AL_PITCH, pitch));
}
// Range [0, +inf]
auto engine::Sound::setVolume(float volume) -> void
{
    volume = std::clamp(volume, 0.f, 99999.f);

    alCall(alSourcef(m_buffer, AL_GAIN, volume));
}

auto engine::Sound::setLoop(bool loop) -> void { alCall(alSourcei(m_buffer, AL_LOOPING, loop ? AL_TRUE : AL_FALSE)); }

auto engine::Sound::getStatus() const -> SoundStatus
{
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
    float result;
    alCall(alGetSourcef(m_buffer, AL_PITCH, &result));
    return result;
}

auto engine::Sound::getVolume() const -> float
{
    float result;
    alCall(alGetSourcef(m_buffer, AL_GAIN, &result));
    return result;
}
auto engine::Sound::doesLoop() const -> bool
{
    ALint result;
    alCall(alGetSourcei(m_buffer, AL_LOOPING, &result));
    return result;
}
