#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

namespace engine {

class Camera {
public:
    Camera();

    auto setCenter(glm::vec2 point) -> void;
    auto setViewport(float left, float right, float bottom, float top) -> void;
    auto setViewportSize(glm::vec2 size) -> void;

    auto move(const glm::vec2 &offset) -> void;

    auto getViewProjMatrix() const -> const glm::mat4 { return m_viewProjMatrix; }
    auto getCenter() -> glm::vec2 { return m_pos; }
    auto getViewportSize() -> glm::vec2 { return m_viewportSize; };

private:
    void recomputeProj();
    void recomputeViewProj();

private:
    glm::mat4 m_projMatrix;
    glm::mat4 m_viewMatrix;

    // result cache
    glm::mat4 m_viewProjMatrix;

    glm::vec2 m_pos{0, 0};
    glm::vec2 m_viewportSize;
};

} // namespace engine
