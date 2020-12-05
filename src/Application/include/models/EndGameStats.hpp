#pragma once

#include <entt/entt.hpp>

namespace game {

struct EndGameStats {
	EndGameStats(entt::registry &world, const entt::entity player);

	int finalLevel;
	double gameTimeInSeconds;
};


}
