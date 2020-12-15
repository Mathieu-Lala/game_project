#pragma once

#include <concepts>
#include <memory>
#include <chrono>
#include <vector>

#include <entt/entt.hpp>
#include <glm/matrix.hpp>

#include "Engine/resources/LoaderColor.hpp"
#include "Engine/resources/LoaderVBOTexture.hpp"
#include "Engine/resources/LoaderTexture.hpp"

#include "Engine/Event/Event.hpp"
#include "Engine/Settings.hpp"
#include "Engine/audio/AudioManager.hpp"

namespace engine {

namespace api {

class Game;

} // namespace api

class Window;
class JoystickManager;
class Shader;
class AudioManager;
struct Settings;

class Core {
private:
    struct hidden_type {
    };

public:
    // note : an Holder should NEVER be declared as global variable
    struct Holder {
        static auto init() noexcept -> Holder;

        Core *instance = Core::s_instance;
    };

    explicit Core(hidden_type &&);

    ~Core();

    Core(const Core &) = delete;
    Core(Core &&) = delete;

    Core &operator=(const Core &) = delete;
    Core &operator=(Core &&) = delete;


    auto main(int argc, char **argv) -> int;


    auto getNextEvent() -> Event;

    auto setPendingEvents(std::vector<Event> &&events) -> void;

    auto setPendingEventsFromFile(const std::string_view filepath) -> bool;


    template<typename... Args>
    auto window(Args &&... args) -> std::unique_ptr<Window> &
    {
        if (m_window != nullptr) { return m_window; }

        m_window = std::make_unique<Window>(std::forward<Args>(args)...);
        loadOpenGL();

        return m_window;
    }

    template<std::derived_from<api::Game> UserDefinedGame, typename... Args>
    auto game(Args &&... args) -> std::unique_ptr<api::Game> &
    {
        if (m_game != nullptr) { return m_game; }

        m_game = std::make_unique<UserDefinedGame>(std::forward<Args>(args)...);

        return m_game;
    }


    enum class EventMode {
        RECORD,
        PLAYBACK,
        PAUSED,
    };

    auto close() noexcept -> void { m_is_running = false; }


    // note : not the best way ..
    auto updateView(const glm::mat4 &view) -> void;

    // todo : add strength
    auto setScreenshake(bool, std::chrono::milliseconds = {}) -> void;


    auto window() noexcept -> std::unique_ptr<Window> & { return m_window; }

    [[nodiscard]] auto getEventMode() const noexcept { return m_eventMode; }
    auto setEventMode(EventMode mode) noexcept { m_eventMode = mode; }

    [[nodiscard]] auto isRunning() const noexcept -> bool { return m_is_running; }

    template<typename T>
    auto getCache() noexcept -> entt::resource_cache<T> &;

    auto getAudioManager() noexcept -> AudioManager & { return m_audioManager; }

    auto getWorld() noexcept -> entt::registry & { return m_world; }

    auto settings() const noexcept -> const Settings & { return m_settings; }

#ifndef NDEBUG
    [[nodiscard]] constexpr auto isShowingDebugInfo() noexcept -> bool { return m_show_debug_info; }
#endif
    auto getJoystick(int id) -> std::optional<Joystick *const>;

private:
    static auto get() noexcept -> std::unique_ptr<Core> &;

    static Core *s_instance;

    static auto loadOpenGL() -> void;

    bool m_is_running{true};

    Settings m_settings;

    // note : for now the engine support only one window
    std::unique_ptr<Window> m_window{nullptr};

    std::unique_ptr<api::Game> m_game{nullptr};

    entt::registry m_world;

    EventMode m_eventMode{EventMode::RECORD};

    std::vector<Event> m_eventsPlayback;


    std::chrono::steady_clock::time_point m_lastTick;

    [[nodiscard]] auto getElapsedTime() noexcept -> std::chrono::nanoseconds;

    auto tickOnce(const TimeElapsed &) -> void;

    std::unique_ptr<JoystickManager> m_joystickManager;

    entt::resource_cache<Color> m_colors;
    entt::resource_cache<VBOTexture> m_vbo_textures;
    entt::resource_cache<Texture> m_textures;

    std::unique_ptr<Shader> m_shader_colored;
    std::unique_ptr<Shader> m_shader_colored_textured;

    AudioManager m_audioManager;

#ifndef NDEBUG
    bool m_show_debug_info = false;

    auto debugDrawJoystick() -> void;
    auto debugDrawDisplayOptions() -> void;
#endif

    std::uint32_t m_displayMode = 4; // note : = GL_TRIANGLES
};

template<>
auto Core::getCache() noexcept -> entt::resource_cache<Color> &;

template<>
auto Core::getCache() noexcept -> entt::resource_cache<VBOTexture> &;

template<>
auto Core::getCache() noexcept -> entt::resource_cache<Texture> &;

} // namespace engine

#ifndef NDEBUG
#    define IF_NOT_PLAYBACK(...)                                                                                       \
        do {                                                                                                           \
            if (engine::Core::Holder{}.instance->getEventMode() != engine::Core::EventMode::PLAYBACK) { __VA_ARGS__; } \
        } while (0)
#else
#    define IF_NOT_PLAYBACK(...) \
        do {                     \
            __VA_ARGS__;         \
        } while (0)
#endif
