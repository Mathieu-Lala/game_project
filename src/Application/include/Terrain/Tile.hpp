#pragma once

#include <entt/entt.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "Engine/component/Drawable.hpp"
#include "Declaration.hpp"

class Tile {
public:
    Tile(entt::registry &world, glm::vec2 coord, glm::vec3 color);

private:
    engine::Drawable createDrawableComponent(glm::vec2 pos, glm::vec3 color);


private:
    // TODO: singleton or something ?
    std::shared_ptr<engine::Shader> _simpleColorShader;
};
