#include <Engine/Graphics/Window.hpp>
#include <Engine/component/Color.hpp>
#include <Engine/component/VBOTexture.hpp>
#include <Engine/Core.hpp>

#include "models/Spell.hpp"

#include "ThePURGE.hpp"
#include "menu/AMenu.hpp"

void game::AMenu::onEvent(entt::registry &world, ThePURGE &game, const engine::Event &e)
{
    static auto holder = engine::Core::Holder{};

    if (!m_createCalled) {
        m_createCalled = true;
        create(world, game);
    }

    if (m_shouldResetInputs) {
        resetInputs();
        m_shouldResetInputs = false;
    }


    std::visit(
        engine::overloaded{
            [&](const engine::Pressed<engine::Key> &key) {
                switch (key.source.key) {
                case GLFW_KEY_W:
                case GLFW_KEY_Z:
                case GLFW_KEY_UP: m_up = true; break;
                case GLFW_KEY_D:
                case GLFW_KEY_RIGHT: m_right = true; break;
                case GLFW_KEY_S:
                case GLFW_KEY_DOWN: m_down = true; break;
                case GLFW_KEY_Q:
                case GLFW_KEY_A:
                case GLFW_KEY_LEFT: m_left = true; break;
                case GLFW_KEY_ENTER: m_select = true; break;
                case GLFW_KEY_ESCAPE: m_close = true; break;
                default: break;
                }
            },
            [&](const engine::Pressed<engine::JoystickButton> &joy) {
                switch (joy.source.button) {
                case engine::Joystick::UP: m_up = true; break;
                case engine::Joystick::DOWN: m_down = true; break;
                case engine::Joystick::LEFT: m_left = true; break;
                case engine::Joystick::RIGHT: m_right = true; break;
                case engine::Joystick::ACTION_BOTTOM: m_select = true; break;
                default: break;
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
                    m_recoveringUp = (!m_recoveringUp && m_up) || (m_recoveringUp && y > kRecoveryThreshold);

                    m_down = !m_recoveringDown && y < -kTriggerThreshold;
                    m_recoveringDown = (!m_recoveringDown && m_down) || (m_recoveringDown && y < -kRecoveryThreshold);

                    m_left = !m_recoveringLeft && x < -kTriggerThreshold;
                    m_recoveringLeft = (!m_recoveringLeft && m_left) || (m_recoveringLeft && x < -kRecoveryThreshold);

                    m_right = !m_recoveringRight && x > kTriggerThreshold;
                    m_recoveringRight = (!m_recoveringRight && m_right) || (m_recoveringRight && x > kRecoveryThreshold);
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
    if (m_shouldResetInputs) resetInputs();

    m_shouldResetInputs = true;

    if (!m_createCalled) {
        m_createCalled = true;
        create(world, game);
    }

    draw(world, game);
}

void game::AMenu::resetInputs() noexcept
{
    m_up = false;
    m_down = false;
    m_left = false;
    m_right = false;
    m_select = false;
    m_close = false;
}
