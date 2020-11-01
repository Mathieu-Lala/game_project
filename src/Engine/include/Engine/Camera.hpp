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

    [[nodiscard]] auto getViewProjMatrix() const noexcept -> const glm::mat4 & { return m_viewProjMatrix; }
    [[nodiscard]] auto getCenter() const noexcept -> glm::vec2 { return m_pos; }
    [[nodiscard]] auto getViewportSize() const noexcept -> glm::vec2 { return m_viewportSize; };

    [[nodiscard("this function will set is_updated to false")]] auto isUpdated() -> bool
    {
        auto out = is_updated;
        is_updated = false;
        return out;
    }

private:
    void recomputeProj();
    void recomputeViewProj();

private:
    bool is_updated{ false };

    glm::mat4 m_projMatrix;
    glm::mat4 m_viewMatrix;

    // result cache
    glm::mat4 m_viewProjMatrix;

    glm::vec2 m_pos{0, 0};
    glm::vec2 m_viewportSize;
};

} // namespace engine
