#pragma once

#include <string>

namespace engine {

struct Settings {

    bool fullscreen;
    std::string replay_path;
    std::string data_folder = "data/";
    std::string config_path;

};

} // namespace engine
