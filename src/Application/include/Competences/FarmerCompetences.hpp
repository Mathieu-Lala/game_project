#pragma once

#include <iostream>

class FarmerCompetences
{
public:
    FarmerCompetences(){};

    // TODO -> Transform function calls in signals 

    void activateSkill(/*Add player if some of it stats are used*/)
    {
        std::cout << s_name << " is used !" << std::endl;
    };

    void displayInfos()
    { 
        std::cout << "Name: " << s_name << std::endl;
        std::cout << "Description: " << s_description << std::endl;
        std::cout << "Cooldown: " << s_cooldown.count() << std::endl;
        std::cout << "Range: " << s_range << std::endl;
    };

    bool m_isOnCooldown{false};

    static inline std::string s_name{"Shovel blow"};
    static inline std::string s_description{"Competence description"};

    static constexpr std::chrono::duration<double> s_cooldown{5};
    static constexpr std::uint32_t s_range{2};

private:

    // Add animation spritesheet
};
