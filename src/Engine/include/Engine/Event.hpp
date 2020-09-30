#pragma once

#include <array>
#include <chrono>
#include <string_view>
#include <variant>

namespace engine {

// Event Helper

template<typename Source>
struct Pressed {
    constexpr static std::string_view name{ "Pressed" };
    constexpr static std::array elements{ std::string_view{ "source" } };
    Source source;
};

template<typename Source>
struct Released {
    constexpr static std::string_view name{ "Released" };
    constexpr static std::array elements{ std::string_view{ "source" } };
    Source source;
};

template<typename Source>
struct Moved {
    constexpr static std::string_view name{ "Moved" };
    constexpr static std::array elements{ std::string_view{ "source" } };
    Source source;
};

template<typename Source>
struct Connected {
    constexpr static std::string_view name{ "Connected" };
    constexpr static std::array elements{ std::string_view{ "source" } };
    Source source;
};

template<typename Source>
struct Disconnected {
    constexpr static std::string_view name{ "Disconnected" };
    constexpr static std::array elements{ std::string_view{ "source" } };
    Source source;
};

// Event Type

/// Window Related

struct CloseWindow {
    constexpr static std::string_view name{ "CloseWindow" };
    constexpr static std::array<std::string_view, 0> elements{};
};

struct OpenWindow {
    constexpr static std::string_view name{ "OpenWindow" };
    constexpr static std::array<std::string_view, 0> elements{};
};

struct ResizeWindow {
    constexpr static std::string_view name{ "ResizeWindow" };
    constexpr static auto elements =
        std::to_array<std::string_view>({ "width", "height" });
    int width;
    int height;
};

struct MoveWindow { // note : could use Moved<Window> ?
    constexpr static std::string_view name{ "MoveWindow" };
    constexpr static auto elements =
        std::to_array<std::string_view>({ "x", "y" });
    int x;
    int y;
};

struct TimeElapsed {
    constexpr static std::string_view name{ "TimeElapsed" };
    constexpr static std::array elements{ std::string_view{ "elapsed" } };
    std::chrono::steady_clock::duration elapsed;
};

/// Device Related

struct Key {
    constexpr static std::string_view name{ "Key" };
    constexpr static auto elements = std::to_array<std::string_view>({
        "alt", "control", "system", "shift", "scancode", "key" });
    bool alt;
    bool control;
    bool system;
    bool shift;

// todo : normalize this
    int scancode;
    int key;
};

struct Mouse {
    constexpr static std::string_view name{ "Mouse" };
    constexpr static auto elements = std::to_array<std::string_view>({ "x", "y" });
    double x;
    double y;
};

struct MouseButton {
    constexpr static std::string_view name{ "MouseButton" };
    constexpr static auto elements = std::to_array<std::string_view>({ "button", "mouse" });
    int button;
    Mouse mouse;
};

struct Joysticks {
    constexpr static std::string_view name{ "Joysticks" };
    constexpr static auto elements = std::to_array<std::string_view>({ "id", "axes", "buttons" });

    enum Axis {
        LSX, // left stick X
        LSY, // left stick Y
        LST, // left shoulder trigger

        RSX, // right stick X
        RSY, // right stick Y
        RST, // right shoulder trigger

        AXES_MAX = 6,
    };

    enum Buttons {

        ACTION_BOTTOM, // A
        ACTION_RIGHT, // B
        ACTION_LEFT, // X
        ACTION_TOP, // Y

        LS, // left shoulder button
        RS, // right shoulder button

        CENTER1, // Back
        CENTER2, // Start

#ifndef WIN32 // Windows override the buttons so we can't use it on Windows
        CENTER3, // Center (xbox home)
#endif
        LSB, // left stick button
        RSB, // right stick button

        UP, // d-pad top
        RIGHT, // d-pad right
        DOWN, // d-pad down
        LEFT, // d-pad left

        NOT_MAPPED, // not used

        BUTTONS_MAX,
    };

    int id;
    std::array<float, AXES_MAX> axes{};
    std::array<std::uint8_t, BUTTONS_MAX> buttons{};

};

// EventType

using Event = std::variant<
    std::monostate,

    OpenWindow,
    CloseWindow,
    ResizeWindow,
    MoveWindow,

    TimeElapsed,

    Pressed<Key>,
    Released<Key>,

    Moved<Mouse>,
    Pressed<MouseButton>,
    Released<MouseButton>,

    Connected<Joysticks>,
    Disconnected<Joysticks>

// todo :
//    Pressed<JoystickButton>,
//    Released<JoystickButton>,
//    Moved<JoystickAxis>,

>;

} // namespace engine

#include "Engine/details/EventJson.hpp"
