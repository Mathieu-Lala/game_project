#include "Engine/audio/WavReader.hpp"
#include <bit>
#include <fstream>
#include <spdlog/spdlog.h>

static std::int32_t convert_to_int(char *buffer, std::size_t len)
{
    std::int32_t a = 0;
    if constexpr (std::endian::native == std::endian::little)
        std::memcpy(&a, buffer, len);
    else
        for (std::size_t i = 0; i < len; ++i) { reinterpret_cast<char *>(&a)[3 - i] = buffer[i]; }
    return a;
}

bool load_wav_file_header(
    std::ifstream &file, std::uint8_t &channels, std::int32_t &sampleRate, std::uint8_t &bitsPerSample, ALsizei &size)
{
    char buffer[4];
    if (!file.is_open()) return false;

    // the RIFF
    if (!file.read(buffer, 4)) { return false; }
    if (std::strncmp(buffer, "RIFF", 4) != 0) { return false; }

    // the size of the file
    if (!file.read(buffer, 4)) { return false; }

    // the WAVE
    if (!file.read(buffer, 4)) { return false; }
    if (std::strncmp(buffer, "WAVE", 4) != 0) { return false; }

    // "fmt/0"
    if (!file.read(buffer, 4)) { return false; }

    // this is always 16, the size of the fmt data chunk
    if (!file.read(buffer, 4)) { return false; }

    // PCM should be 1?
    if (!file.read(buffer, 2)) { return false; }

    // the number of channels
    if (!file.read(buffer, 2)) { return false; }
    channels = static_cast<uint8_t>(convert_to_int(buffer, 2));

    // sample rate
    if (!file.read(buffer, 4)) { return false; }
    sampleRate = convert_to_int(buffer, 4);

    // (sampleRate * bitsPerSample * channels) / 8
    if (!file.read(buffer, 4)) { return false; }

    // ?? dafaq
    if (!file.read(buffer, 2)) { return false; }

    // bitsPerSample
    if (!file.read(buffer, 2)) { return false; }
    bitsPerSample = static_cast<uint8_t>(convert_to_int(buffer, 2));

    // data chunk header "data"
    if (!file.read(buffer, 4)) { return false; }
    if (std::strncmp(buffer, "data", 4) != 0) { return false; }

    // size of data
    if (!file.read(buffer, 4)) { return false; }
    size = convert_to_int(buffer, 4);

    /* cannot be at the end of file */
    if (file.eof()) { return false; }
    if (file.fail()) { return false; }

    return true;
}

auto engine::load_wav(
    const std::string_view filename, std::uint8_t &channels, std::int32_t &sampleRate, std::uint8_t &bitsPerSample, ALsizei &size)
    -> char *
{
    std::ifstream in(filename.data(), std::ios::binary);
    if (!in.is_open()) {
        spdlog::error("ERROR: Could not open '{}'", filename);
        return nullptr;
    }
    if (!load_wav_file_header(in, channels, sampleRate, bitsPerSample, size)) {
        spdlog::error("ERROR: Could not load wav header of '{}'", filename);
        return nullptr;
    }

    auto data = new char[static_cast<std::size_t>(size)];

    in.read(data, size);

    return data;
}
