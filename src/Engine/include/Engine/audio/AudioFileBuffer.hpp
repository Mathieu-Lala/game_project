#pragma once

#include <string>
#include <AL/al.h>

namespace engine {

class AudioFileBuffer {
public:
    explicit AudioFileBuffer(const std::string_view path);
    ~AudioFileBuffer();

    constexpr
    auto get() const noexcept -> ALuint { return m_buffer; }

private:
    ALuint m_buffer;
};

} // namespace engine
