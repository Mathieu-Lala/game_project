#include <Engine/Event/Event.hpp>        // note : should not require this header here
#include <Engine/Graphics/Shader.hpp>    // note : should not require this header here
#include <Engine/audio/AudioManager.hpp> // note : should not require this header here
#include <Engine/Graphics/Window.hpp>    // note : should not require this header here
#include <Engine/Settings.hpp>           // note : should not require this header here
#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/Core.hpp>

// tmp
#include "models/Spell.hpp"
#include "ThePURGE.hpp"


#include <spdlog/spdlog.h>

int main(int argc, char **argv) try
{
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::trace);
#else
    spdlog::set_level(spdlog::level::warn);
#endif

    auto holder = engine::Core::Holder::init();

    holder.instance->game<game::ThePURGE>();

    return holder.instance->main(argc, argv);
}
catch (int code)
{
    return code;
}
