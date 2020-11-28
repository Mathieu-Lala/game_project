#pragma once

#include <glm/vec2.hpp>
#include <entt/entt.hpp>

#include <Engine/Event/Event.hpp>

namespace game {

class ThePURGE;

struct MenuTexture {
    std::uint32_t id;

    // In a range [0; 1] relative to the window size
    ImVec2 topleft;
    ImVec2 size;
};


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


    //
    // Input helpers with most common interractions.
    // Should only be read during the draw() function
    //
    bool up() const noexcept { return m_up; }
    bool down() const noexcept { return m_down; }
    bool right() const noexcept { return m_right; }
    bool left() const noexcept { return m_left; }
    // User closed menu
    bool close() const noexcept { return m_close; }
    // User selected / clicked on current option
    bool select() const noexcept { return m_select; }


    void forceUp(bool val) noexcept { m_up = val; }
    void forceDown(bool val) noexcept { m_down = val; }
    void forceLeft(bool val) noexcept { m_left = val; }
    void forceRight(bool val) noexcept { m_right = val; }

    void forceSelect(bool val) noexcept { m_select = val; }
    void forceClose(bool val) noexcept { m_close = val; }


    //
    // Display helpers
    //

    // Input : [0; 1], position in percentage of the window, Output : [0, windowSize] corresponding position in pixels
    auto frac2pixel(ImVec2 fraction) const noexcept -> ImVec2;
    void drawTexture(std::uint32_t id, ImVec2 topLeft, ImVec2 size) const noexcept;
    void drawTexture(const MenuTexture &texture) const noexcept;

private:
    static constexpr float kRecoveryThreshold = 0.3f;
    static constexpr float kTriggerThreshold = 0.3f;

    glm::vec2 m_prevLeftJoystick{0.f, 0.f};

    bool m_createCalled = false;

    bool m_up = false;
    bool m_down = false;
    bool m_left = false;
    bool m_right = false;
    bool m_select = false;
    bool m_close = false;

    bool m_recoveringUp = false;
    bool m_recoveringDown = false;
    bool m_recoveringRight = false;
    bool m_recoveringLeft = false;
};

} // namespace game
