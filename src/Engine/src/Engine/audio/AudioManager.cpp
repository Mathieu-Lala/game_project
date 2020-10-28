#include "Engine/audio/AudioManager.hpp"

#include <fmt/format.h>
#include <algorithm>
#include <spdlog/spdlog.h>

#include "Engine/audio/AlErrorHandling.hpp"

engine::AudioManager::AudioManager()
{
    m_device = alcOpenDevice(nullptr);

    if (!m_device) {
        spdlog::warn("Could not open audio device, no audio will be playing");
        return;
    }

    alcCall(m_device, m_context = alcCreateContext(m_device, nullptr));

    if (!m_context) throw std::runtime_error("Could not create audio context");

    ALboolean success = AL_FALSE;
    alcCall(m_device, success = alcMakeContextCurrent(m_context));

    if (success == AL_FALSE) throw std::runtime_error("ERROR: Could not make audio context current");
}

engine::AudioManager::~AudioManager()
{
    if (!m_device)
        return;

    for (auto &s : m_currentSounds) s->forceDestroy(); // destroys sounds even if user still have references
    m_audioFileCache.clear();

    alcCall(m_device, alcDestroyContext(m_context));
    alcCloseDevice(m_device);
}

auto engine::AudioManager::getSound(const std::string &path) -> std::shared_ptr<Sound>
{
    if (!m_device)
        return Sound::GetEmptySound();

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
