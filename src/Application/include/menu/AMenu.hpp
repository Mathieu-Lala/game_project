#pragma once

#include <glm/vec2.hpp>
#include <entt/entt.hpp>

#include <Engine/Event/Event.hpp>

namespace game {

class ThePURGE;

class AMenu {
public:
    AMenu() = default;
    virtual ~AMenu() = default;

    void onEvent(entt::registry &world, ThePURGE &game, const engine::Event &e);
    void onDraw(entt::registry &world, ThePURGE &game);

protected:
    virtual void create(entt::registry &, ThePURGE &) {};
    virtual void event(entt::registry &, ThePURGE &, const engine::Event &) = 0;
    virtual void draw(entt::registry &, ThePURGE &) = 0;

    bool up() const noexcept { return m_up; }
    bool down() const noexcept { return m_down; }
    bool right() const noexcept { return m_right; }
    bool left() const noexcept { return m_left; }

    // Input : [0; 1], position in percentage of the window, Output : [0, windowSize] corresponding position in pixels
    auto frac2pixel(ImVec2 fraction) const noexcept -> ImVec2;
    void drawTexture(std::uint32_t id, ImVec2 topLeft, ImVec2 size) const noexcept;

private:
    static constexpr float kRecoveryThreshold = 0.4f;
    static constexpr float kTriggerThreshold = 0.6f;

    glm::vec2 m_prevLeftJoystick{0.f, 0.f};

    bool m_createCalled = false;

    bool m_up = false;
    bool m_down = false;
    bool m_left = false;
    bool m_right = false;

    bool m_recoveringUp = false;
    bool m_recoveringDown = false;
    bool m_recoveringRight = false;
    bool m_recoveringLeft = false;
};

} // namespace game
