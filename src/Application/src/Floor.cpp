#include "Terrain/Floor.hpp"
#include <GL/glew.h>
#include "Engine/component/Drawable.hpp"


Floor::Floor(entt::registry &world, const glm::vec2 &pos) : Tile(world, pos, kColor) {}
