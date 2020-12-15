#include <fstream>

#include <spdlog/spdlog.h>

#include "models/Enemy.hpp"

bool game::EnemyDatabase::fromFile(const std::string_view path)
{
    std::ifstream file(path.data());
    if (!file.is_open()) {
        spdlog::error("Can't open the given file");
        return false;
    }
    const auto jsonData = nlohmann::json::parse(file);

    this->db = jsonData.get<std::decay_t<decltype(this->db)>>();

    return true;
}
