#pragma once

#include <string>

namespace engine {

struct Settings {
    std::string config_path;

    std::string replay_path;
    std::string replay_data;
    std::string data_folder;
    std::string output_folder;

    bool fullscreen;
    std::uint16_t window_width;
    std::uint16_t window_height;
};

} // namespace engine
