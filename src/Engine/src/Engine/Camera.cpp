#include "Engine/Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

engine::Camera::Camera() : _projMatrix(1.f)
{
    update();
}

auto engine::Camera::setCenter(::glm::vec2 point) -> void
{
    _pos.x = point.x;
    _pos.y = point.y;

    update();
}

auto engine::Camera::setViewport(float left, float right, float bottom, float top) -> void
{
    _projMatrix = glm::ortho(left, right, bottom, top, _pos.z, 99999.f);
    _pos.x = (left + right) / 2.0f;
    _pos.y = (top + bottom) / 2.0f;

    update();
}

auto engine::Camera::update() -> void
{
    auto transform = glm::translate(glm::mat4(1.f), _pos);

    _viewMatrix = glm::inverse(transform);

    _viewProjMatrix = _projMatrix * _viewMatrix;
}
