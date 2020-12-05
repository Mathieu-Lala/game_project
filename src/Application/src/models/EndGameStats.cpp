#include "models/EndGameStats.hpp"

#include "component/all.hpp"

game::EndGameStats::EndGameStats(entt::registry &world, const entt::entity player, double gameTime) {

	finalLevel = world.get<Level>(player).current_level;

	gameTimeInSeconds = gameTime;

	enemyKilled = world.get<StatsTracking>(player).enemyKilled;
}
