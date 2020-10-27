#include <stdexcept>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "Engine/audio/AudioFileBuffer.hpp"
#include "Engine/audio/WavReader.hpp"
#include "Engine/audio/AlErrorHandling.hpp"

engine::AudioFileBuffer::AudioFileBuffer(const std::string_view path)
{
    std::uint8_t channels;
    std::int32_t sampleRate;
    std::uint8_t bitsPerSample;

    struct databuff {
        ALsizei size;
        char *data;

        ~databuff() { delete[] data; }
    } soundData;

    soundData.data = load_wav(path, channels, sampleRate, bitsPerSample, soundData.size);
    if (!soundData.data) throw std::runtime_error(fmt::format("Could not load audio file '{}'", path));

    alCall(alGenBuffers(1, &m_buffer));

    ALenum format;
    if (channels == 1 && bitsPerSample == 8)
        format = AL_FORMAT_MONO8;
    else if (channels == 1 && bitsPerSample == 16)
        format = AL_FORMAT_MONO16;
    else if (channels == 2 && bitsPerSample == 8)
        format = AL_FORMAT_STEREO8;
    else if (channels == 2 && bitsPerSample == 16)
        format = AL_FORMAT_STEREO16;
    else
        throw std::runtime_error(
            fmt::format("ERROR: unrecognised wave format: {} channels, {} bits per sample", channels, bitsPerSample));


    alCall(alBufferData(m_buffer, format, soundData.data, soundData.size, sampleRate));
}


engine::AudioFileBuffer::~AudioFileBuffer() { alCall(alDeleteBuffers(1, &m_buffer)); }
