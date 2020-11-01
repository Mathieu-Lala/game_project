#pragma once

#include <string>
#include <AL/al.h>

namespace engine {

// copy-pasta from https://indiegamedev.net/2020/02/15/the-complete-guide-to-openal-with-c-part-1-playing-a-sound/
auto load_wav(
    const std::string_view filename, std::uint8_t &channels, std::int32_t &sampleRate, std::uint8_t &bitsPerSample, ALsizei &size)
    -> char *;

} // namespace engine
