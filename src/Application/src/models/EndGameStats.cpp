#include "models/EndGameStats.hpp"

#include "component/all.hpp"

game::EndGameStats::EndGameStats(entt::registry &world, const entt::entity player) {

	finalLevel = world.get<Level>(player).current_level;

	gameTimeInSeconds = 132.45; // TODO: actual value
}
