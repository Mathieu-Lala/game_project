#include <iostream>
#include <chrono>

#include <classes/Classes.hpp>
#include <classes/ClassFactory.hpp>

game::ClassFactory::ClassFactory(
    Classes cl,
    const std::string &desc,
    const double &cooldown,
    const std::uint32_t &range,
    const std::uint32_t &damage,
    const bool attackType) :
    s_isRangedAttack{attackType},
    s_name{[](Classes c) {
        switch (c) {
        case Classes::FARMER: return "Farmer";
        case Classes::SHOOTER: return "Shooter";
        case Classes::SOLDIER: return "Soldier";
        case Classes::SORCERER: return "Sorcerer";
        }
        return "";
    }(cl)},
    s_description{desc}, s_cooldown{cooldown}, s_range{range}, s_damage{damage}
{
}

auto game::ClassFactory::activateSkill() -> void { std::cout << s_name << " is used !" << std::endl; }

auto game::ClassFactory::displayInfos() -> void
{
    std::cout << "Name: " << s_name << std::endl;
    std::cout << "Description: " << s_description << std::endl;
    std::cout << "Cooldown: " << s_cooldown << std::endl;
    std::cout << "Range: " << s_range << std::endl;
    std::cout << "Damage: " << s_damage << std::endl;
}
