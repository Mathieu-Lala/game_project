#pragma once

#include <entt/entt.hpp>

#include <glm/vec3.hpp>

namespace engine {

class Game {
public:
    virtual ~Game() = default;

    virtual auto onCreate(entt::registry &) -> void = 0;

    virtual auto onDestroy(entt::registry &) -> void = 0;

    virtual auto onUpdate(entt::registry &, const Event &) -> void = 0;

    virtual auto drawUserInterface(entt::registry &) -> void = 0;

    virtual auto getBackgroundColor() const noexcept -> glm::vec3 = 0;

};

} // namespace engine
