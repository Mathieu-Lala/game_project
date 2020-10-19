#include "Engine/audio/AlErrorHandling.hpp"
#include <stdexcept>
#include <spdlog/spdlog.h>

bool engine::checkForAlcError(ALCdevice *device)
{
    ALCenum error = alcGetError(device);
    if (error != ALC_NO_ERROR) {
        switch (error) {
        case ALC_INVALID_VALUE:
            spdlog::error("ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function");
            break;
        case ALC_INVALID_DEVICE:
            spdlog::error("ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function");
            break;
        case ALC_INVALID_CONTEXT:
            spdlog::error("ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function");
            break;
        case ALC_INVALID_ENUM:
            spdlog::error("ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function");
            break;
        case ALC_OUT_OF_MEMORY:
            spdlog::error("ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function");
            break;
        default: spdlog::error("UNKNOWN ALC ERROR: {}", error);
        }
        return true;
    }
    return false;
}

bool engine::checkForAlError()
{
    ALenum error = alGetError();

    if (error != AL_NO_ERROR) {
        switch (error) {
        case AL_INVALID_NAME: spdlog::error("AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function"); break;
        case AL_INVALID_ENUM:
            spdlog::error("AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function");
            break;
        case AL_INVALID_VALUE:
            spdlog::error("AL_INVALID_VALUE: an invalid value was passed to an OpenAL function");
            break;
        case AL_INVALID_OPERATION: spdlog::error("AL_INVALID_OPERATION: the requested operation is not valid"); break;
        case AL_OUT_OF_MEMORY:
            spdlog::error("AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory");
            break;
        default: spdlog::error("UNKNOWN AL ERROR: {}", error);
        }
        return true;
    }
    return false;
}