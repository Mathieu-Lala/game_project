#pragma once

namespace engine {

class Game {
public:

    virtual ~Game() = default;

    virtual auto onCreate(entt::registry &) -> void = 0;
    virtual auto onUpdate(entt::registry &) -> void = 0;
    virtual auto onDestroy(entt::registry &) -> void = 0;

};

} // namespace engine
