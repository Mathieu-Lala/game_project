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
        std::cout << std::filesystem::current_path().string() << std::endl;
        module = engine::dll::Handler(std::filesystem::current_path().string() + "\\ThePURGE.dll");
    } catch (const engine::dll::Handler::error &e) {
        std::cout << e.what() << std::endl;
    }

    auto constructor = module.load<engine::api::Game *(*)()>("constructor");

    holder.instance->game(std::unique_ptr<engine::api::Game>(constructor()));

    return holder.instance->main(argc, argv);
}
