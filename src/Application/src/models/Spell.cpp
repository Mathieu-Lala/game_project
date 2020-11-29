#include <chrono>
#include <optional>
#include <fstream>

#include <spdlog/spdlog.h>

#include <Engine/component/Cooldown.hpp>

#include "factory/SpellFactory.hpp"
#include "models/Spell.hpp"

using namespace std::chrono_literals;

auto game::SpellDatabase::instantiate(const std::string_view spell) -> std::optional<Spell>
{
    if (const auto found = std::find_if(std::begin(db), std::end(db), [&spell](auto &i) { return i.first == spell; });
        found != std::end(db)) {
        return Spell{
            .id = found->first,
            .cd = {
                .is_in_cooldown = false,
                .cooldown = found->second.cooldown,
                .remaining_cooldown = 0ms,
            }};

    } else {
        spdlog::error("SpellDatabase::instantiate: No such spell '{}'", spell.data());
        return {};
    }
}

auto game::SpellDatabase::fromFile(const std::string_view path) -> bool
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
