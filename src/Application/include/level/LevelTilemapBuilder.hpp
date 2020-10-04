#pragma once

#include <cstddef>
#include <entt/entt.hpp>
#include <glm/vec2.hpp>

enum class TileEnum : unsigned char {
    NONE = 0,
    WALL,
    FLOOR,
};

class TilemapBuilder {
public:

    TilemapBuilder(glm::ivec2 &&size = { 100, 100 }) :
        m_size(size), m_tiles(static_cast<std::size_t>(m_size.x * m_size.y), TileEnum::NONE)
    {
    }

    auto get(int x, int y) -> TileEnum &;

    void build(entt::registry &world);

    auto getSize() const -> const glm::ivec2 & { return m_size; }

private:
    void handleTileBuild(entt::registry &world, int x, int y);
    glm::ivec2 getTileSize(int x, int y);

private:

    const glm::ivec2 m_size;
    std::vector<TileEnum> m_tiles;
};
