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

int main(int argc, char **argv)
{
    auto holder = engine::Core::Holder::init();

    holder.instance->game<game::ThePURGE>();

    return holder.instance->main(argc, argv);
}
