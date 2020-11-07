#include <spdlog/spdlog.h>

#include "Engine/Graphics/third_party.hpp"
#include "Engine/Event/JoystickManager.hpp"
#include "Engine/Graphics/Shader.hpp"    // note : should not require this header here
#include "Engine/audio/AudioManager.hpp" // note : should not require this header here
#include "Engine/Settings.hpp"           // note : should not require this header here
#include "Engine/component/Color.hpp"
#include "Engine/component/Texture.hpp"
#include "Engine/Core.hpp"

engine::JoystickManager *engine::JoystickManager::s_instance{nullptr};

engine::JoystickManager::JoystickManager()
{
    s_instance = this;

    ::glfwSetJoystickCallback(callback_eventJoystickDetection);
}

auto engine::JoystickManager::getNextEvent() -> std::optional<Event>
{
    if (m_events.empty()) {
        return {};
    } else {
        auto nextEvent = m_events.front();
        m_events.erase(m_events.begin());
        return nextEvent;
    }
}

auto engine::JoystickManager::add(const Joystick &j) -> void
{
    if (!get(j.id).has_value())
        m_joysticks.push_back(j);
    else
        [[unlikely]] { spdlog::warn("engine::JoystickManager Trying to add an already existing joystick"); }
}

auto engine::JoystickManager::get(int id) -> std::optional<Joystick *const>
{
    if (const auto it = std::find_if(m_joysticks.begin(), m_joysticks.end(), [id](auto &i) { return i.id == id; });
        it != m_joysticks.end()) {
        return &(*it);
    } else {
        return {};
    }
}

auto engine::JoystickManager::poll() -> void
{
    pollAxis();
    pollButtons();
}

auto engine::JoystickManager::remove(const Joystick &j) -> void
{
    if (auto it = std::find_if(m_joysticks.begin(), m_joysticks.end(), [id = j.id](auto &i) { return i.id == id; });
        it != m_joysticks.end()) {
        m_joysticks.erase(it);
    } else
        [[unlikely]] { spdlog::warn("engine::JoystickManager Trying to remove an none existing joystick"); }
}

auto engine::JoystickManager::update(const Moved<JoystickAxis> &j) -> void
{
    if (auto it = std::find_if(m_joysticks.begin(), m_joysticks.end(), [id = j.source.id](auto &i) { return i.id == id; });
        it != m_joysticks.end()) {
        it->axes[j.source.axis] = j.source.value;
    }
}

auto engine::JoystickManager::update(const Pressed<JoystickButton> &j) -> void
{
    if (auto it = std::find_if(m_joysticks.begin(), m_joysticks.end(), [id = j.source.id](auto &i) { return i.id == id; });
        it != m_joysticks.end()) {
        it->buttons[j.source.button] = true;
    }
}

auto engine::JoystickManager::update(const Released<JoystickButton> &j) -> void
{
    if (auto it = std::find_if(m_joysticks.begin(), m_joysticks.end(), [id = j.source.id](auto &i) { return i.id == id; });
        it != m_joysticks.end()) {
        it->buttons[j.source.button] = false;
    }
}

auto engine::JoystickManager::each(const std::function<void(const Joystick &)> &f) -> void
{
    for (const auto &i : m_joysticks) f(i);
}

auto engine::JoystickManager::callback_eventJoystickDetection(int id, int event) -> void
{
    if (Core::Holder().instance->getEventMode() == Core::EventMode::RECORD) {
        if (event == GLFW_CONNECTED) {
            s_instance->m_events.emplace_back(Connected<Joystick>{{id}});
        } else if (event == GLFW_DISCONNECTED) {
            s_instance->m_events.emplace_back(Disconnected<Joystick>{{id}});
        }
    }
}

auto engine::JoystickManager::pollAxis() -> void
{
    int count = 0;
    for (auto &joy : m_joysticks) {
        const auto axes = ::glfwGetJoystickAxes(joy.id, &count);
        for (std::uint32_t i = 0; i != static_cast<std::uint32_t>(count); i++) {
            if (std::abs(joy.axes[i] - axes[i]) >= 0.05f) {
                this->m_events.emplace_back(
                    Moved<JoystickAxis>({joy.id, magic_enum::enum_cast<Joystick::Axis>(i).value(), axes[i]}));
            }
        }
    }
}

auto engine::JoystickManager::pollButtons() -> void
{
    int count = 0;
    for (auto &joy : m_joysticks) {
        const auto buttons = ::glfwGetJoystickButtons(joy.id, &count);
        for (std::uint32_t i = 0; i != static_cast<std::uint32_t>(count); i++) {
            if (joy.buttons[i] != static_cast<bool>(buttons[i])) {
                if (buttons[i]) {
                    this->m_events.emplace_back(
                        Pressed<JoystickButton>({joy.id, magic_enum::enum_cast<Joystick::Buttons>(i).value()}));
                } else {
                    this->m_events.emplace_back(
                        Released<JoystickButton>({joy.id, magic_enum::enum_cast<Joystick::Buttons>(i).value()}));
                }
            }
        }
    }
}
