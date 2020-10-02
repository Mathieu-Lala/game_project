#pragma once

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include "Engine/details/Graphics.hpp"

namespace engine {

class Camera {
public:
    Camera();

    auto setZPos(float z) { _pos.z = z; }
    auto setCenter(::glm::vec2 point) -> void;
    auto setViewport(float left, float right, float bottom, float top) -> void;

    auto getViewProjMatrix() const -> const ::glm::mat4 { return _viewProjMatrix; }

private:
    void update();

private:
    ::glm::mat4 _projMatrix;
    ::glm::mat4 _viewMatrix;

    // result cache
    ::glm::mat4 _viewProjMatrix;

    ::glm::vec3 _pos{0, 0, 0};
};

} // namespace engine
