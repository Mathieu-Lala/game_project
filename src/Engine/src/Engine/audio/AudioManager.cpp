#include "Engine/audio/AudioManager.hpp"

#include <fmt/format.h>
#include <algorithm>
#include <spdlog/spdlog.h>

#include "Engine/audio/WavReader.hpp"
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
    m_currentSounds.clear(); // destroys most sounds

    alcCall(m_device, alcDestroyContext(m_context));
    alcCloseDevice(m_device);
}

auto engine::AudioManager::getSound(const std::string &path) -> std::shared_ptr<Sound>
{
    spdlog::info("Loading sound '{}'", path);

    garbageCollectCurrentSounds(); // We should run this method periodically. calling here is the easiest way for now

    // TODO: cache + proper delete via `alDeleteSources(1, &buffer)`
    auto buffer = genSoundBuffer(path);

    auto sound = std::make_shared<Sound>(buffer);
    m_currentSounds.push_back(sound);

    return sound;
}

auto engine::AudioManager::genSoundBuffer(const std::string &path) -> ALuint
{
    std::uint8_t channels;
    std::int32_t sampleRate;
    std::uint8_t bitsPerSample;

    struct databuff {
        ALsizei size;
        char *data;

        ~databuff() {
            delete[] data;
        }
    } soundData;

    soundData.data = load_wav(path, channels, sampleRate, bitsPerSample, soundData.size);
    if (!soundData.data)
        throw std::runtime_error(fmt::format("Could not load audio file '{}'", path));

    ALuint buffer;
    alCall(alGenBuffers(1, &buffer));

    ALenum format;
    if(channels == 1 && bitsPerSample == 8)
        format = AL_FORMAT_MONO8;
    else if(channels == 1 && bitsPerSample == 16)
        format = AL_FORMAT_MONO16;
    else if(channels == 2 && bitsPerSample == 8)
        format = AL_FORMAT_STEREO8;
    else if(channels == 2 && bitsPerSample == 16)
        format = AL_FORMAT_STEREO16;
    else
        throw std::runtime_error(fmt::format(
            "ERROR: unrecognised wave format: {} channels, {} bits per sample", channels, bitsPerSample));


    alCall(alBufferData(buffer, format, soundData.data, soundData.size, sampleRate));

    return buffer;
}

void engine::AudioManager::garbageCollectCurrentSounds()
{
    m_currentSounds.erase(
        std::remove_if(
            std::begin(m_currentSounds),
            std::end(m_currentSounds),
            [](auto &s) { return s.use_count() == 1 && s->getStatus() != SoundStatus::PLAYING; }),
        std::end(m_currentSounds));

    spdlog::info("current sound count : {}", m_currentSounds.size());
}
