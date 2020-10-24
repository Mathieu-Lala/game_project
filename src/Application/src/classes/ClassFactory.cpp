#include <iostream>
#include <chrono>

#include <classes/Classes.hpp>
#include <classes/ClassFactory.hpp>

engine::ClassFactory::ClassFactory(engine::Classes cl, const std::string &desc, const double &cooldown, const std::uint32_t &range, const std::uint32_t &damage, const bool attackType)
    : s_description{desc}, s_cooldown{cooldown}, s_range{range}, s_damage{damage}, s_isRangedAttack{attackType}
{
    switch(cl) {
        case engine::Classes::FARMER: s_name = "Farmer"; break;
        case engine::Classes::SHOOTER: s_name = "Shooter"; break;
        case engine::Classes::SOLDIER: s_name = "Soldier"; break;
        case engine::Classes::SORCERER: s_name = "Sorcerer"; break;
    }
};

auto engine::ClassFactory::activateSkill() -> void
{
    std::cout << s_name << " is used !" << std::endl;
};

auto engine::ClassFactory::displayInfos() -> void
{ 
    std::cout << "Name: " << s_name << std::endl;
    std::cout << "Description: " << s_description << std::endl;
    std::cout << "Cooldown: " << s_cooldown << std::endl;
    std::cout << "Range: " << s_range << std::endl;
    std::cout << "Damage: " << s_damage << std::endl;
};
