#pragma once

#include "Engine/Camera.hpp"
#include "Engine/Event.hpp"

namespace engine {

class Game {
public:

    virtual ~Game() = default;

    virtual auto onCreate(entt::registry &) -> void = 0;
    virtual auto onDestroy(entt::registry &) -> void = 0;

    // tmp
    virtual auto onUpdate(entt::registry &) -> void = 0;

    // tmp
    virtual auto onEvent(const Event &) -> void = 0;

    auto getCamera() -> Camera & { return m_camera;}

private:
    Camera m_camera;
};

} // namespace engine
