#pragma once

#include <entt/entt.hpp>

#include <glm/vec4.hpp>

#include "Engine/Event/Event.hpp"

namespace engine {

namespace api {

class Game {
public:
    virtual ~Game() = default;

    /**
     * function called when the game is created
     */
    virtual auto onCreate(entt::registry &) -> void = 0;

    /**
     * function called at the game is destroyed
     */
    virtual auto onDestroy(entt::registry &) -> void = 0;

    /**
     * function called at every frame with an engine event
     */
    virtual auto onUpdate(entt::registry &, const Event &) -> void = 0;

    /**
     * function called at every frame, should call the GUI rendering
     */
    virtual auto drawUserInterface(entt::registry &) -> void = 0;

    /**
     * function called at every frame, return the clear color
     */
    virtual auto getBackgroundColor() const noexcept -> glm::vec4 = 0;

};

} // namespace api

} // namespace engine
