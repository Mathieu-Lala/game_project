#pragma once

#include <array>
#include <optional>
#include <string_view>
#include <string>
#include <fstream>
#include <iostream>

#include <CLI/CLI.hpp>

#include "Engine/details/Version.hpp"
#include "Engine/Settings.hpp"

namespace engine {

static constexpr auto NAME = "ThePURGE " PROJECT_VERSION;

static constexpr auto VERSION = PROJECT_NAME " - ThePURGE - " PROJECT_VERSION " - "
#ifndef NDEBUG
                                             "Debug"
#else
                                             "Release"
#endif
    ;

struct Options {
    static constexpr auto DEFAULT_DATA_FOLDER = "data";
    static constexpr auto DEFAULT_CONFIG = "data/config/app.ini";

    enum Value { CONFIG_PATH, FULLSCREEN, REPLAY_PATH, DATA_FOLDER, OPTION_MAX };

    std::array<CLI::Option *, OPTION_MAX> options;

    Options(int argc, char **argv) : app(NAME, argv[0])
    {
        app.add_flag(
            "--version",
            [](auto v) -> void {
                if (v == 1) {
                    std::cout << VERSION << "\n";
                    std::exit(0);
                }
            },
            "Print the version number and exit.");

        options[CONFIG_PATH] = app.set_config("--config", DEFAULT_CONFIG);
        options[FULLSCREEN] =
            app.add_option("--fullscreen", settings.fullscreen, "Launch the window in fullscreen mode.", true);
        options[REPLAY_PATH] = app.add_option("--replay-path", settings.replay_path, "Path of the events to replay.");
        options[DATA_FOLDER] = app.add_option("--data", settings.data_folder, "Path of the data folder");

        if (const auto res = [&]() -> std::optional<int> {
                CLI11_PARSE(app, argc, argv);
                return {};
            }();
            res.has_value()) {
            this->setDefaultValue();
            throw res.value_or(0);
        }
    }

    auto setDefaultValue() -> void
    {
        if (!options[Options::CONFIG_PATH]->empty())
            settings.config_path = options[Options::CONFIG_PATH]->as<std::string>();
        if (!options[Options::DATA_FOLDER]->empty())
            settings.data_folder = options[Options::DATA_FOLDER]->as<std::string>();
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

    Settings settings{
        .fullscreen = true, .replay_path = "", .data_folder = DEFAULT_DATA_FOLDER, .config_path = DEFAULT_CONFIG};
};

} // namespace engine
