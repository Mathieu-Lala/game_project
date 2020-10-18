#pragma once

#include "AL/al.h"

namespace engine {
class Sound {
public:
    Sound(ALuint soundBuffer);
    ~Sound();

    auto play() -> void;
private:
    ALuint m_buffer;

};
} // namespace engine