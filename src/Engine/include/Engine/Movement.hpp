#pragma once

#include <iostream>

namespace engine {
    
    enum Direction {
        NORTH,
        EAST,
        SOUTH,
        WEST,
    };

    class Movement
    {
    public:
        void moveAxis(/* &Player */ Direction direction)
        { 
            switch (direction) {
            case engine::Direction::NORTH:
                std::cout << "UP" << std::endl;
                break;
            case engine::Direction::EAST:
                std::cout << "RIGHT" << std::endl;
                break;
            case engine::Direction::SOUTH:
                std::cout << "DOWN" << std::endl;
                break;
            case engine::Direction::WEST:
                std::cout << "LEFT" << std::endl;
                break;
            default: break;
            }

            return;
        };
    };
}
