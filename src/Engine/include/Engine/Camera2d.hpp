#pragma once

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "Engine/details/Graphics.hpp"

namespace engine {

class Camera2d {
public:
    Camera2d();

    auto setZ(float z) -> void;
    auto setCenter(glm::vec2 point) -> void;
    auto setViewport(float left, float right, float bottom, float top) -> void;
    auto setViewportSize(glm::vec2 size) -> void;

    auto move(const glm::vec2 &offset) -> void;

    auto getViewProjMatrix() const -> const glm::mat4 { return m_viewProjMatrix; }
    auto getCenter() -> glm::vec2 { return {m_pos.x, m_pos.y}; }
    auto getZ() { return m_pos.z; }
    auto getViewportSize() -> const glm::vec2 & { return m_viewportSize; };

private:
    void recomputeProj();
    void recomputeViewProj();

private:
    // not working. not really needed anyway
    static constexpr float kMaxZView = 999999;

    glm::mat4 m_projMatrix;
    glm::mat4 m_viewMatrix;

    // result cache
    glm::mat4 m_viewProjMatrix;

    glm::vec3 m_pos{0, 0, 0};
    glm::vec2 m_viewportSize;
};

} // namespace engine
