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
    static constexpr std::size_t kMaxWidth = 200;
    static constexpr std::size_t kMaxHeight = 200;

    auto get(int x, int y) -> TileEnum &;

    void build(entt::registry & world);

private:
    void handleTileBuild(entt::registry &world, int x, int y);
    glm::vec2 getTileSize(int x, int y);

private:
    std::array<TileEnum, kMaxHeight * kMaxWidth> m_tiles;
};
