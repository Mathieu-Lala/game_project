#pragma once

#define Z_COMPONENT_OF(e) (static_cast<double>(e))

enum EntityDepth {
	PLAYER,
	ENEMIES,
	TERRAIN,

	MAX_VAL
};
