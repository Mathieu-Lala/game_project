#pragma once

#include <vector>

#include "Engine/Event/Event.hpp"

namespace engine {

class Core;

class JoystickManager {
public:
    JoystickManager();

    auto add(const Joystick &j) -> void;
    auto remove(const Joystick &j) -> void;

    auto get(int id) -> std::optional<Joystick *const>;
    auto each(const std::function<void(const Joystick &)> &f) -> void;

    auto getNextEvent() -> std::optional<Event>;
    auto poll() -> void;

    auto update(const Moved<JoystickAxis> &j) -> void;
    auto update(const Pressed<JoystickButton> &j) -> void;
    auto update(const Released<JoystickButton> &j) -> void;

private:
    std::vector<Joystick> m_joysticks;

    std::vector<Event> m_events;

    static JoystickManager *s_instance;

    static auto callback_eventJoystickDetection(int id, int event) -> void;

    auto pollAxis() -> void;
    auto pollButtons() -> void;
};

} // namespace engine
