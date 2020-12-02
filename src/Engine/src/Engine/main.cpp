#include <Engine/Event/Event.hpp>        // note : should not require this header here
#include <Engine/Graphics/Shader.hpp>    // note : should not require this header here
#include <Engine/audio/AudioManager.hpp> // note : should not require this header here
#include <Engine/Graphics/Window.hpp>    // note : should not require this header here
#include <Engine/Settings.hpp>           // note : should not require this header here
#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/Core.hpp>

#include <Engine/dll/Handler.hpp>

#include <spdlog/spdlog.h>

//// tmp
//#include "models/Spell.hpp"
//#include "ThePURGE.hpp"

#include <filesystem>
#include <iostream>

int main(int argc, char **argv)
{
    spdlog::set_level(spdlog::level::trace);

    auto holder = engine::Core::Holder::init();

    // holder.instance->game<game::ThePURGE>();
    engine::dll::Handler module;
    try {
        const auto module_name =
#if defined(_WIN32)
#    ifdef NDEBUG
            "\\ThePURGE.dll"
#    else
            "\\ThePURGE-d.dll"
#    endif
#else
#    ifdef NDEBUG
            "/libThePURGE.so"
#    else
            "/libThePURGE-d.so"
#    endif
#endif
            ;

        module = engine::dll::Handler(std::filesystem::current_path().string() + module_name);
    } catch (const engine::dll::Handler::error &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }

    auto constructor = module.load<engine::api::Game *(*) ()>("constructor");

    holder.instance->game(std::unique_ptr<engine::api::Game>(constructor()));

    return holder.instance->main(argc, argv);
}
