#pragma once

#include <glm/vec2.hpp>

namespace game {

/*
	Dictate the direction player is casting spells

	Probably just the input of the right joystick
*/
struct Facing {
	glm::vec2 dir; // normalized 
};

} // namespace game
