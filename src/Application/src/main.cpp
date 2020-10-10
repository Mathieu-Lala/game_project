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

#define STB_IMAGE_IMPLEMENTATION

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

class Controller {
public:
    void printA(const std::string &str) { std::cout << str << std::endl; };
    void printB(const std::string &str) { std::cout << str << std::endl; };
    void printX(const std::string &str) { std::cout << str << std::endl; };
    void printY(const std::string &str) { std::cout << str << std::endl; };
};

int main(int argc, char **argv) try
{
    // todo : setup properly logging
    auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
    logger->info("logger created");


    // 1. Parse the program argument

    Options opt{argc, argv};

    
    engine::Core::Holder holder;

    // holder.instance->window(glm::ivec2{400, 400}, "The PURGE");
    // holder.instance->game<ThePurge>();

    // ...
    // entt::sigh<void(int, char)> signal;
    // entt::sink sink{signal};
    // listener instance;
    // boop test;

    // sink.connect<&foo>();
    // sink.connect<&listener::bar>(instance);
    // sink.connect<&boop::printTest>(test);

    std::string buffer{};

    Controller controller;
    FarmerCompetences farmer;

    entt::sigh<void(void)> actionBottom;
    entt::sigh<void(void)> actionRight;
    entt::sigh<void(const std::string &)> actionLeft;
    entt::sigh<void(const std::string &)> actionTop;

    entt::sink sinkB(actionBottom);
    entt::sink sinkR(actionRight);
    entt::sink sinkL(actionLeft);
    entt::sink sinkT(actionTop);

    sinkB.connect<&FarmerCompetences::activateSkill>(farmer);
    sinkR.connect<&FarmerCompetences::displayInfos>(farmer);
    sinkL.connect<&Controller::printX>(controller);
    sinkT.connect<&Controller::printY>(controller);

    while (std::getline(std::cin, buffer)) {
        std::cout << "Input -> " << buffer << std::endl;
        if (buffer == "A" || buffer == "ACTION_BOTTOM") {
            actionBottom.publish();
        } else if (buffer == "B" || buffer == "ACTION_RIGHT") {
            actionRight.publish();
        } else if (buffer == "X" || buffer == "ACTION_LEFT") {
            actionLeft.publish("X is Pressed !");
        } else if (buffer == "Y" || buffer == "ACTION_RIGHT") {
            actionTop.publish("Y is Pressed !");
        } else if (buffer == "exit")
            break;
    }

    //// ...
    // signal.publish(42, 'c');

    //// disconnects a free function
    // sink.disconnect<&foo>();

    //// disconnect a member function of an instance
    // sink.disconnect<&listener::bar>(instance);

    //// disconnect all member functions of an instance, if any
    // sink.disconnect(instance);

    //// discards all listeners at once
    // sink.disconnect();

    std::uint16_t windowProperty = engine::Window::Property::DEFAULT;
    if (opt.fullscreen) windowProperty |= engine::Window::Property::FULLSCREEN;

    //holder.instance->window(glm::ivec2{400, 400}, VERSION, windowProperty);
    //holder.instance->game<ThePurge>();


    // 3. Apply optional argument and run

#ifndef NDEBUG
    opt.dump();
#endif

    opt.write_to_file(!opt.options[Options::CONFIG_PATH]->empty()
        ? opt.options[Options::CONFIG_PATH]->as<std::string>()
        : Options::DEFAULT_CONFIG);


    // 2. Initialize the Engine / Window / Game

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
