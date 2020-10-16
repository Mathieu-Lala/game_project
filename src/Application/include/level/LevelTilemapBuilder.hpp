#pragma once

#include <cstddef>
#include <entt/entt.hpp>
#include <glm/vec2.hpp>

#include "Engine/Graphics/Shader.hpp"

namespace game {

#define IS_FLOOR(tile) \
    ((tile == game::TileEnum::FLOOR_SPAWN \
    || tile == game::TileEnum::FLOOR_CORRIDOR \
    || tile == game::TileEnum::FLOOR_BOSS_ROOM \
    || tile == game::TileEnum::FLOOR_NORMAL_ROOM))

enum class TileEnum : std::uint8_t {
    NONE = 0,
    RESERVED, // not a real tile, for internal use of the generation algorithm
    DEBUG_TILE, // visual to ease debug

    WALL,
    EXIT_DOOR_FACING_NORTH,
    EXIT_DOOR_FACING_EAST,
    EXIT_DOOR_FACING_SOUTH,
    EXIT_DOOR_FACING_WEST,

    FLOOR_CORRIDOR,
    FLOOR_SPAWN,
    FLOOR_BOSS_ROOM,
    FLOOR_NORMAL_ROOM,
};

class TilemapBuilder {
public:
    TilemapBuilder(engine::Shader *shader, glm::ivec2 &&size = {100, 100}) :
        m_shader(shader), m_size(size),
        m_tiles(static_cast<std::size_t>(m_size.x) * static_cast<std::size_t>(m_size.y), TileEnum::NONE)
    {
    }

    auto get(int x, int y) -> TileEnum &;

    void build(entt::registry &world);

    auto getSize() const -> const glm::ivec2 & { return m_size; }

private:
    void handleTileBuild(entt::registry &world, int x, int y);
    glm::ivec2 getTileSize(int x, int y);

private:
    engine::Shader *m_shader;

    const glm::ivec2 m_size;
    std::vector<TileEnum> m_tiles;
};

} // namespace game
