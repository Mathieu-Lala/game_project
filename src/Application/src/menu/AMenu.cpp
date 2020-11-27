#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/Core.hpp>

#include "ThePURGE.hpp"
#include "menu/AMenu.hpp"

void game::AMenu::onEvent(entt::registry &world, ThePURGE &game, const engine::Event &e)
{
    static auto holder = engine::Core::Holder{};

    std::visit(
        engine::overloaded{
            [&](const engine::Pressed<engine::Key> &key) {
                switch (key.source.key) {
                case GLFW_KEY_UP: m_up = true; break;
                case GLFW_KEY_RIGHT: m_right = true; break;
                case GLFW_KEY_DOWN: m_down = true; break;
                case GLFW_KEY_LEFT: m_left = true; break;

                default: break;
                }
            },
            [&](const engine::Pressed<engine::JoystickButton> &joy) {
                switch (joy.source.button) {
                case engine::Joystick::UP: m_up = true; break;
                case engine::Joystick::DOWN: m_down = true; break;
                case engine::Joystick::LEFT: m_left = true; break;
                case engine::Joystick::RIGHT: m_right = true; break;
                default: return;
                }
            },
            [&](const engine::Moved<engine::JoystickAxis> &joy) {
                switch (joy.source.axis) {
                case engine::Joystick::LSX:
                case engine::Joystick::LSY: {
                    auto joystick = holder.instance->getJoystick(joy.source.id);

                    float x = (*joystick)->axes[engine::Joystick::LSX];
                    float y = -(*joystick)->axes[engine::Joystick::LSY];

                    m_up = !m_recoveringUp && y > kTriggerThreshold;
                    m_recoveringUp =
                        (!m_recoveringUp && y > kTriggerThreshold) || (m_recoveringUp && y > kTriggerThreshold);

                    m_down = !m_recoveringDown && y > kTriggerThreshold;
                    m_recoveringDown =
                        (!m_recoveringDown && y > kTriggerThreshold) || (m_recoveringDown && y > kTriggerThreshold);

                    m_left = !m_recoveringLeft && x > kTriggerThreshold;
                    m_recoveringLeft =
                        (!m_recoveringLeft && x > kTriggerThreshold) || (m_recoveringLeft && x > kTriggerThreshold);

                    m_right = !m_recoveringRight && x > kTriggerThreshold;
                    m_recoveringRight =
                        (!m_recoveringRight && x > kTriggerThreshold) || (m_recoveringRight && x > kTriggerThreshold);
                } break;
                default: break;
                }
            },
            [&](auto) {},
        },
        e);

    event(world, game, e);
}

void game::AMenu::onDraw(entt::registry &world, ThePURGE &game)
{
    if (draw(world, game)) {
        m_up = false;
        m_down = false;
        m_left = false;
        m_right = false;
    }
}
