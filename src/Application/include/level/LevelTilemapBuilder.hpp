#pragma once

#include <cstddef>
#include <cassert>

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

#include "Engine/Graphics/Shader.hpp"

namespace game {

// note : avoid macro
#define IS_FLOOR(tile)                                                              \
    ((tile == game::TileEnum::FLOOR_SPAWN || tile == game::TileEnum::FLOOR_CORRIDOR \
      || tile == game::TileEnum::FLOOR_BOSS_ROOM || tile == game::TileEnum::FLOOR_NORMAL_ROOM))

enum class TileEnum : std::uint8_t { // note : should be merged with EntityFactory::ID
    NONE = 0,
    RESERVED,   // not a real tile, for internal use of the generation algorithm
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
    explicit TilemapBuilder(glm::ivec2 &&size = {100, 100}) :
        m_size(size), m_tiles(static_cast<std::size_t>(m_size.x) * static_cast<std::size_t>(m_size.y), TileEnum::NONE)
    {
    }

    template<typename T>
    auto operator[](const glm::vec<2, T> &index) noexcept -> TileEnum &
    {
        assert(index.x < m_size.x);
        assert(index.y < m_size.y);
        return m_tiles[static_cast<std::size_t>(index.y * m_size.x + index.x)];
    }

    template<typename T>
    auto at(const glm::vec<2, T> &index) const -> TileEnum
    {
        assert(index.x < m_size.x);
        assert(index.y < m_size.y);
        return m_tiles.at(static_cast<std::size_t>(index.y * m_size.x + index.x));
    }

    auto build(entt::registry &world) -> void;

    auto getSize() const -> const glm::ivec2 & { return m_size; }

private:
    auto handleTileBuild(entt::registry &world, int x, int y) -> void;
    auto getTileSize(int x, int y) const -> glm::ivec2;

private:
    const glm::ivec2 m_size;
    std::vector<TileEnum> m_tiles;
};

} // namespace game
