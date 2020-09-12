#pragma once

#include <array>
#include <chrono>
#include <string_view>
#include <variant>

#include "Engine/Utility.hpp" // for overloaded

namespace engine {

struct CloseWindow {
    constexpr static std::string_view name{ "CloseWindow" };
    constexpr static std::array<std::string_view, 0> elements{};
};

struct TimeElapsed {
    constexpr static std::string_view name{ "TimeElapsed" };
    constexpr static std::array elements{ std::string_view{ "elapsed" } };
    std::chrono::steady_clock::duration elapsed;
};

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

/////////////////////////////

using Event = std::variant<
    std::monostate,
    CloseWindow,
    TimeElapsed,
    Pressed<Key>,
    Released<Key>
//    Pressed<JoystickButton>,
//    Released<JoystickButton>,
//    Moved<JoystickAxis>,
//    Moved<Mouse>,
//    Pressed<MouseButton>,
//    Released<MouseButton>,
>;

} // namespace engine
