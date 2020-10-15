#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <Engine/Event/Event.hpp>
#include <Engine/Graphics/Window.hpp>
#include <Engine/Core.hpp>

#include "GameLogic.hpp"
#include "ThePURGE.hpp"
#include "Options.hpp"

int main(int argc, char **argv)
try {
    // todo : setup properly logging
    auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
    logger->info("logger created");


    // 1. Parse the program argument

    game::Options opt{argc, argv};

#ifndef NDEBUG
    opt.dump();
#endif

    opt.write_to_file(
        !opt.options[game::Options::CONFIG_PATH]->empty() ? opt.options[game::Options::CONFIG_PATH]->as<std::string>()
                                                          : game::Options::DEFAULT_CONFIG);

    // 2. Initialize the Engine / Window / Game

    auto holder = engine::Core::Holder::init();

    std::uint16_t windowProperty = engine::Window::Property::DEFAULT;
    if (opt.fullscreen) windowProperty |= engine::Window::Property::FULLSCREEN;

    holder.instance->window(glm::ivec2{400, 400}, game::VERSION, windowProperty);
    holder.instance->game<game::ThePurge>();

#ifndef NDEBUG
    if (!opt.options[game::Options::REPLAY_PATH]->empty()) holder.instance->setPendingEventsFromFile(opt.replay_path);
#endif

    // 3. Start of the application

    return holder.instance->main();
} catch (const std::exception &e) {
    spdlog::error("Caught exception at main level: {}", e.what());
    throw;
} catch (int code) {
    return code;
}
