#include <fstream>
#include <streambuf>
#include <entt/entt.hpp>
#include <iostream>
#include <CLI/CLI.hpp>
#include <spdlog/sinks/basic_file_sink.h>

#include <Engine/Core.hpp>
#include <Engine/details/Version.hpp>

#include "ThePurge.hpp"
#include <Declaration.hpp>

#include <Competences/FarmerCompetences.hpp>

static constexpr auto NAME = "ThePURGE " PROJECT_VERSION;

static constexpr auto VERSION = PROJECT_NAME " - ThePURGE - " PROJECT_VERSION
#ifndef NDEBUG
    " - Debug"
#else
    " - Release"
#endif
;

struct Options {

    static constexpr auto DEFAULT_CONFIG = "app.ini";

    enum Value {
        CONFIG_PATH,
        FULLSCREEN,
        REPLAY_PATH,
        OPTION_MAX
    };

    std::array<CLI::Option *, OPTION_MAX> options;

    Options(int argc, char **argv) :
        app(NAME, argv[0])
    {
        app.add_flag("--version", [](auto v) -> void {
            if (v == 1) { std::cout << VERSION; exit(0); } }, "Print the version number and exit.");

        options[CONFIG_PATH] = app.set_config("--config", DEFAULT_CONFIG);
        options[FULLSCREEN] = app.add_option("--fullscreen", fullscreen, "Launch the window in fullscreen mode.", true);
        options[REPLAY_PATH] = app.add_option("--replay_path", replay_path, "Path of the events to replay.");

        if (const auto res = [&]() -> std::optional<int> { CLI11_PARSE(app, argc, argv); return {}; }(); res.has_value()) { throw res.value_or(0); }
    }

    auto dump() const -> void
    {
// todo :
//        spdlog::warn("Working on file: {}, direct count: {}, opt count: {}",
//            fullscreen, app.count("--fullscreen"), options[FULLSCREEN]->count());
    }

    auto write_to_file(const std::string_view path) -> bool
    {
        std::ofstream f(path.data());
        if (!f.is_open()) { return false; }
        f << app.config_to_str(true, true);
        return true;
    }

    CLI::App app;

    bool fullscreen = true;
    std::string replay_path;

};

int main(int argc, char **argv) try
{
    // todo : setup properly logging
    auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
    logger->info("logger created");


    // 1. Parse the program argument

    Options opt{argc, argv};

#ifndef NDEBUG
    opt.dump();
#endif

    opt.write_to_file(!opt.options[Options::CONFIG_PATH]->empty()
        ? opt.options[Options::CONFIG_PATH]->as<std::string>()
        : Options::DEFAULT_CONFIG);

    // 2. Initialize the Engine / Window / Game

    auto holder = engine::Core::Holder::init();

    std::uint16_t windowProperty = engine::Window::Property::DEFAULT;

    holder.instance->window(glm::ivec2{400, 400}, VERSION, windowProperty);
    holder.instance->game<ThePurge>();

#ifndef NDEBUG
    if (!opt.options[Options::REPLAY_PATH]->empty())
        holder.instance->setPendingEventsFromFile(opt.replay_path);
#endif

    return holder.instance->main();
}
catch (const std::exception &e)
{
    spdlog::error("Caught exception at main level: {}", e.what());
}
catch (int code)
{
    return code;
}
