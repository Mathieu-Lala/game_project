#include "Engine/audio/AudioManager.hpp"

#include <fmt/format.h>
#include <algorithm>
#include <spdlog/spdlog.h>

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
    m_audioFileCache.clear();

    alcCall(m_device, alcDestroyContext(m_context));
    alcCloseDevice(m_device);
}

auto engine::AudioManager::getSound(const std::string &path) -> std::shared_ptr<Sound>
{
    garbageCollectCurrentSounds(); // We should run this method periodically. calling here is the easiest way for now

    auto buffer = m_audioFileCache.load<AudioFileLoader>(entt::hashed_string(path.c_str()).value(), path);

    auto sound = std::make_shared<Sound>(buffer->get());
    m_currentSounds.push_back(sound);

    return sound;
}

void engine::AudioManager::garbageCollectCurrentSounds()
{
    m_currentSounds.erase(
        std::remove_if(
            std::begin(m_currentSounds),
            std::end(m_currentSounds),
            [](auto &s) { return s.use_count() == 1 && s->getStatus() != SoundStatus::PLAYING; }),
        std::end(m_currentSounds));
}
