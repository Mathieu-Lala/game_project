#pragma once

#include <iostream>
#include <chrono>

#include "Classes.hpp"

namespace game {

class /*[[deprecated]]*/ ClassFactory {
public:
    ClassFactory(
        Classes cl,
        const std::string &desc,
        const double &cooldown,
        const std::uint32_t &range,
        const std::uint32_t &damage,
        const bool attackType);

    auto activateSkill() -> void;
    auto displayInfos() -> void;

    bool m_isOnCooldown{false};

    bool s_isRangedAttack{false};

    std::string s_name{""};
    std::string s_description{""};

    // std::chrono::duration<double> s_cooldown{0};
    double s_cooldown{0};
    std::uint32_t s_range{0};
    std::uint32_t s_damage{0};

private:
};

} // namespace game
