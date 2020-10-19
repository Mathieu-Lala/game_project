#pragma once

#include <string>
#include <AL/al.h>

namespace engine {

class AudioFileBuffer {
public:
	explicit AudioFileBuffer(const std::string &path);
	~AudioFileBuffer();

	auto get() -> ALuint { return m_buffer;} 

private:
	ALuint m_buffer;
};
}