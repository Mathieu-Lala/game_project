#pragma once

#include <array>
#include <chrono>
#include <string_view>
#include <variant>

#include <nlohmann/json.hpp>

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
    constexpr static auto elements = std::to_array<std::string_view>({ "id" });
    int id;
//    float *axes;
//    unsigned char *buttons;

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

#include "Engine/EventJson.hpp"
