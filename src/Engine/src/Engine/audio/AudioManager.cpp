#include "Engine/audio/AudioManager.hpp"

#include <fmt/format.h>

#include "Engine/audio/AudioFile.h"
#include "Engine/audio/AlErrorHandling.hpp"

engine::AudioManager::AudioManager()
{
    m_device = alcOpenDevice(nullptr);

    if (!m_device) throw std::runtime_error("Could not open audio device");

    alcCall(m_device, m_context = alcCreateContext(m_device, nullptr));

    if (!m_context) throw std::runtime_error("Could not create audio context");

    ALboolean success = AL_FALSE;
    alcCall(m_device, success = alcMakeContextCurrent(m_context));

    if (success == AL_FALSE) throw std::runtime_error("ERROR: Could not make audio context current");
}

engine::AudioManager::~AudioManager()
{
    m_currentSounds.clear(); // destroys all current sound
        
    alcCall(m_device, alcDestroyContext(m_context));
    alcCall(m_device, alcCloseDevice(m_device));
}

auto engine::AudioManager::getSound(const std::string &path) -> std::shared_ptr<Sound>
{
    // TODO: cache + proper delete via `alDeleteSources(1, &buffer)`
    auto buffer = genSoundBuffer(path);

    auto sound = std::make_shared<Sound>(buffer);
    m_currentSounds.push_back(sound);

    return sound;
}

auto engine::AudioManager::genSoundBuffer(const std::string &path) -> ALuint
{
    AudioFile<double> file;

    if (!file.load(path)) throw std::runtime_error(fmt::format("Could not load audio file '{}'", path));


    ALuint buffer;
    alCall(alGenBuffers(1, &buffer));

    ALenum format;
    if (file.getNumChannels() == 1 && file.getBitDepth() == 8)
        format = AL_FORMAT_MONO8;
    else if (file.getNumChannels() == 1 && file.getBitDepth() == 16)
        format = AL_FORMAT_MONO16;
    else if (file.getNumChannels() == 2 && file.getBitDepth() == 8)
        format = AL_FORMAT_STEREO8;
    else if (file.getNumChannels() == 2 && file.getBitDepth() == 16)
        format = AL_FORMAT_STEREO16;
    else
        throw std::runtime_error(fmt::format(
            "ERROR: unrecognised wave format: {} channels, {} bps", file.getNumChannels(), file.getBitDepth()));

    alCall(alBufferData(buffer, format, file.samples.data(), file.samples.size() * sizeof(file.samples[0]), file.getSampleRate()));

    return buffer;
}
