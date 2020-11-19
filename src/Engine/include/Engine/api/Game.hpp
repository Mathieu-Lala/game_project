#pragma once

#include <entt/entt.hpp>

#include <glm/vec3.hpp>

#include "Engine/Event/Event.hpp"

namespace engine {

namespace api {

class Game {
public:
    virtual ~Game() = default;

    /**
     * function called at the beginning of the game
     */
    virtual auto onCreate(entt::registry &) -> void = 0;

    /**
     * function called at the end of the game
     */
    virtual auto onDestroy(entt::registry &) -> void = 0;

    /**
     * function called at every frame with an engine event
     */
    virtual auto onUpdate(entt::registry &, const Event &) -> void = 0;

    /**
     * function called for the rendering of the GUI
     */
    virtual auto drawUserInterface(entt::registry &) -> void = 0;

    /**
     * function called at every frame, return the clear color
     */
    virtual auto getBackgroundColor() const noexcept -> glm::vec3 = 0;

};

} // namespace api

} // namespace engine
