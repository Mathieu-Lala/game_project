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
    alCall(alSourcei(m_buffer, AL_BUFFER, soundBuffer));
}

engine::Sound::~Sound()
{
    alSourceStop(m_buffer);
    alCall(alDeleteSources(1, &m_buffer));
}

void engine::Sound::play()
{
    spdlog::info("Playing sound !");
    alCall(alSourcePlay(m_buffer));
}
