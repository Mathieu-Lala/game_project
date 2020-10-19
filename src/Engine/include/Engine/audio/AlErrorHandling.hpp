#pragma once

/*
    Macro utilies to check for OpenAl errors
*/

#include <AL/al.h>
#include <AL/alc.h>

namespace engine {

#ifdef _MSC_VER
#    define DEBUG_BREAK __debugbreak();
#else
#    define DEBUG_BREAK
#endif // _MSC_VER

#define alcCall(device, call)                          \
    do {                                               \
        while (alcGetError(device) != AL_NO_ERROR)     \
            ;                                          \
        call;                                          \
        if (checkForAlcError(device)) { DEBUG_BREAK; } \
    } while (0);

#define alCall(call)                            \
    do {                                        \
        while (alGetError() != AL_NO_ERROR)     \
            ;                                   \
        call;                                   \
        if (checkForAlError()) { DEBUG_BREAK; } \
    } while (0);

bool checkForAlcError(ALCdevice *device);
bool checkForAlError();

} // namespace engine