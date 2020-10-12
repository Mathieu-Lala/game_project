#include "Engine/details/Warnings.hpp"

DISABLE_WARNING_PUSH
DISABLE_WARNING_CONSTANT_CONDITIONAL
#include <glm/gtc/matrix_transform.hpp>
DISABLE_WARNING_POP

#include <spdlog/spdlog.h>
#include "Engine/Camera2d.hpp"

// Default viewport ranges from -1 to +1
engine::Camera2d::Camera2d() { setViewportSize(glm::ivec2{2, 2}); }


auto engine::Camera2d::setCenter(::glm::vec2 point) -> void
{
    m_pos.x = point.x;
    m_pos.y = point.y;

    recomputeViewProj();
}

auto engine::Camera2d::setViewport(float left, float right, float bottom, float top) -> void
{
    assert(right > left);
    assert(top > bottom);

    m_pos.x = (left + right) / 2.0f;
    m_pos.y = (top + bottom) / 2.0f;
    m_viewportSize.x = right - left;
    m_viewportSize.y = top - bottom;

    recomputeProj();
    recomputeViewProj();
}

auto engine::Camera2d::setViewportSize(glm::vec2 size) -> void
{
    assert(size.x > 0);
    assert(size.y > 0);

    m_viewportSize = size;

    recomputeProj();
    recomputeViewProj();
}


auto engine::Camera2d::move(const glm::vec2 &offset) -> void
{
    m_pos.x += offset.x;
    m_pos.y += offset.y;

    recomputeViewProj();
}

void engine::Camera2d::recomputeProj()
{
    auto halfVP = m_viewportSize / 2.f;

    m_projMatrix = glm::ortho(
        - halfVP.x, // left
        + halfVP.x, // right
        - halfVP.y, // bottom
        + halfVP.y, // top
        99999999.f,
        -99999999.f);
}

auto engine::Camera2d::recomputeViewProj() -> void
{
    auto transform = glm::translate(glm::mat4(1.f), glm::vec3(m_pos.x, m_pos.y, 0));

    m_viewMatrix = glm::inverse(transform);

    m_viewProjMatrix = m_projMatrix * m_viewMatrix;
}
