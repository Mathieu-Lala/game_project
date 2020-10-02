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
    TilemapBuilder(int maxWidth = 100, int maxHeight = 100) :
        m_maxWidth(maxWidth), m_maxHeight(maxHeight), m_tiles(m_maxWidth * m_maxHeight, TileEnum::NONE)
    {}

    auto get(int x, int y) -> TileEnum &;

    void build(entt::registry & world);

    auto getMaxWidth() const { return m_maxWidth; }
    auto getMaxHeight() const { return m_maxHeight; }

private:
    void handleTileBuild(entt::registry &world, int x, int y);
    glm::vec2 getTileSize(int x, int y);

private:
    std::vector<TileEnum> m_tiles;
    int m_maxWidth;
    int m_maxHeight;
};
