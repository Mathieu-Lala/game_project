#pragma once

#include <cstddef>
#include <entt/entt.hpp>
#include <glm/vec2.hpp>

#include "Engine/Graphics/Shader.hpp"

namespace game {

// note : this should be a component ?
enum class TileEnum : std::uint8_t {
    NONE = 0,
    WALL,
    FLOOR,
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
