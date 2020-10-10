#pragma once

#include <iostream>

namespace engine {
    enum Direction {
        UP,
        RIGHT,
        DOWN,
        LEFT
    };

    class Movement
    {
    public:
        void moveAxis(/* &Player */ Direction direction)
        { 
            switch (direction) {
            case Direction::UP: std::cout << "UP" << std::endl; break;
            case Direction::RIGHT: std::cout << "RIGHT" << std::endl; break;
            case Direction::DOWN: std::cout << "DOWN" << std::endl; break;
            case Direction::LEFT: std::cout << "LEFT" << std::endl; break;
            default: break;
            }

            return;
        };
    };
}
