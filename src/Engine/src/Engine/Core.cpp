#include <fstream>
#include <functional>
#include <filesystem>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <magic_enum.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Engine/helpers/overloaded.hpp"
#include "Engine/component/Drawable.hpp"
#include "Engine/component/Position.hpp"
#include "Engine/component/Rotation.hpp"
#include "Engine/component/Scale.hpp"
#include "Engine/component/Velocity.hpp"
#include "Engine/component/Acceleration.hpp"
#include "Engine/component/Hitbox.hpp"
#include "Engine/component/Color.hpp"
#include "Engine/component/Spritesheet.hpp"
#include "Engine/component/VBOTexture.hpp"
#include "Engine/component/Lifetime.hpp"

#include "Engine/Event/Event.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Window.hpp"
#include "Engine/Event/JoystickManager.hpp"
#include "Engine/Options.hpp"
#include "Engine/api/Game.hpp"
#include "Engine/audio/AudioManager.hpp" // note : should not require this header here
#include "Engine/Core.hpp"

#include "Engine/helpers/DrawableFactory.hpp"
#include "Engine/helpers/ImGui.hpp"

#include "Engine/Graphics/third_party.hpp" // note : only for DisplayMode

using namespace std::chrono_literals;

engine::Core *engine::Core::s_instance{nullptr};

auto engine::Core::Holder::init() noexcept -> Holder
{
    Core::s_instance = Core::get().get();
    return {};
}

engine::Core::Core([[maybe_unused]] hidden_type &&)
{
    ::glfwSetErrorCallback([](int code, const char *message) {
        spdlog::error("Engine::Core GLFW An error occured '{}' 'code={}'\n", message, code);
    });

    spdlog::trace("Engine::Core instanciated");
    if (::glfwInit() == GLFW_FALSE) { throw std::logic_error(fmt::format("Engine::Core initialization failed")); }

    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    ::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    ::glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    spdlog::trace("Engine::Core GLFW version: '{}'\n", ::glfwGetVersionString());

    IMGUI_CHECKVERSION();

    m_joystickManager = std::make_unique<JoystickManager>();
}

engine::Core::~Core()
{
    ::glfwTerminate();
    ::glfwSetErrorCallback(nullptr);
    spdlog::trace("Engine::Core destroyed");
}

auto engine::Core::setPendingEvents(std::vector<Event> &&events) -> void
{
    m_eventMode = EventMode::PLAYBACK;
    m_eventsPlayback = std::move(events);
}

auto engine::Core::setPendingEventsFromFile(const std::string_view filepath) -> bool
try {
    std::ifstream ifs(filepath.data());
    if (!ifs.is_open()) {
        spdlog::warn("engine::Core setPendingEventsFromFile failed: {} could not be opened", filepath.data());
        return false;
    }
    const auto j = nlohmann::json::parse(ifs);
    setPendingEvents(j.get<std::vector<Event>>());
    return true;
} catch (...) {
    spdlog::warn("engine::Core setPendingEventsFromFile failed: could not parse the file {}", filepath.data());
    return false;
}

auto engine::Core::getNextEvent() -> Event
{
    ::glfwPollEvents();

    switch (m_eventMode) {
    case EventMode::PAUSED:
    case EventMode::RECORD: {
        m_joystickManager->poll();

        // 1. poll the window event
        // 2. poll the joysticks event
        // 3. send elapsed time
        auto event =
            m_window->getNextEvent().value_or(m_joystickManager->getNextEvent().value_or(TimeElapsed{getElapsedTime()}));

        // TEMPORARY, BY YANIS. Allows for keyboard input to work. waiting for Mathieu to help do it a clean way
        std::visit(
            overloaded{
                [&](const auto &e) { m_window->applyEvent(e); },
            },
            event);
        // ----
        return event;
    } break;
    case EventMode::PLAYBACK: {
        if (m_eventsPlayback.empty()) {
            spdlog::info("Engine::Window switching to record mode");
            m_eventMode = EventMode::RECORD;
            return TimeElapsed{getElapsedTime()};
        }
        auto event = m_eventsPlayback.front();
        m_eventsPlayback.erase(m_eventsPlayback.begin());

        std::visit(
            overloaded{
                [&](const ResizeWindow &e) {
                    m_window->setSize({e.width, e.height});
                },
                [&](const MoveWindow &e) {
                    m_window->setPosition({e.x, e.y});
                },
                [](const TimeElapsed &dt) { std::this_thread::sleep_for(dt.elapsed); },
                [&](const Moved<Mouse> &m) {
                    m_window->setCursorPosition({m.source.x, m.source.y});
                },
                [&](const auto &e) { m_window->applyEvent(e); },
            },
            event);
        return event;

    } break;
    default: std::abort();
    }
}

auto engine::Core::main(int argc, char **argv) -> int
{
    {
#ifdef LOGLOGLOG
        // todo : setup properly logging
        auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
        logger->trace("logger created");
#endif

        Options opt{argc, argv};

#ifndef NDEBUG
        opt.dump();
#endif

        opt.write_to_file(opt.settings.config_path + ".last");


#ifndef NDEBUG
        if (!opt.options[Options::REPLAY_PATH]->empty()) {
            setPendingEventsFromFile(opt.settings.replay_path);
        } else if (!opt.options[Options::REPLAY_DATA]->empty()) {
            setPendingEvents(nlohmann::json::parse(opt.settings.replay_data));
        }
#endif

        m_settings = std::move(opt.settings);
    }

    std::uint16_t windowProperty = engine::Window::Property::DEFAULT;
    if (m_settings.fullscreen) windowProperty |= engine::Window::Property::FULLSCREEN;

    this->window(glm::ivec2{m_settings.window_width, m_settings.window_height}, VERSION, windowProperty);

    if (m_window == nullptr || m_game == nullptr) { return 1; }

    m_shader_colored.reset(new Shader{Shader::fromFile(
        m_settings.data_folder + "shaders/colored.vert.glsl", m_settings.data_folder + "shaders/colored.frag.glsl")});

    m_shader_colored_textured.reset(new Shader{Shader::fromFile(
        m_settings.data_folder + "shaders/colored_textured.vert.glsl",
        m_settings.data_folder + "shaders/colored_textured.frag.glsl")});

    // todo : add max size buffer ?
    std::vector<Event> eventsProcessed{TimeElapsed{}};

    auto screenshake = m_world.create();
    m_world.emplace<entt::tag<"screenshake"_hs>>(screenshake);
    m_world.emplace<engine::Cooldown>(screenshake, false, 500ms, 0ms);

    m_game->onCreate(m_world);

    while (isRunning()) {
        const auto event = getNextEvent();

        std::visit(
            overloaded{
                [](TimeElapsed &prev, const TimeElapsed &next) { prev.elapsed += next.elapsed; },
                [&]([[maybe_unused]] const auto &, [[maybe_unused]] const std::monostate &) {},
                // event in playback mode will be saved twice = bad !
                [&]([[maybe_unused]] const auto &, const auto &next) { eventsProcessed.push_back(next); }},
            eventsProcessed.back(),
            event);

        // todo : remove me ?
        bool timeElapsed = false;

        static constexpr auto time_to_string = [](std::time_t now) -> std::string {
            const auto tp = std::localtime(&now);
            char buffer[32];
            return std::strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", tp) ? buffer : "1970-01-01_00:00:00";
        };

        std::visit(
            overloaded{
                [&]([[maybe_unused]] const OpenWindow &) { m_lastTick = std::chrono::steady_clock::now(); },
                [&]([[maybe_unused]] const CloseWindow &) {
                    m_window->close();
                    this->close();
                },
                [&](const ResizeWindow &e) {
                    m_window->setSize({e.width, e.height});
                },
                [&]([[maybe_unused]] const TimeElapsed &) { timeElapsed = true; },
                [&]([[maybe_unused]] const Pressed<Key> &) {
                    // todo : abstract glfw keyboard
                    switch (const auto keyEvent = std::get<Pressed<Key>>(event); keyEvent.source.key) {
                    case GLFW_KEY_ESCAPE: this->close(); break;
#ifndef NDEBUG
                    case GLFW_KEY_F1:
                        m_show_debug_info = !m_show_debug_info;
                        window()->setCursorVisible(m_show_debug_info);
                        break;
                    case GLFW_KEY_F2:
                        m_eventMode = m_eventMode == EventMode::PAUSED ? EventMode::RECORD : EventMode::PAUSED;
                        m_window->setCursorVisible(m_eventMode == EventMode::PAUSED);
                        break;

#endif
                    case GLFW_KEY_F11: m_window->setFullscreen(!m_window->isFullscreen()); break;
                    case GLFW_KEY_F12: {
                        std::filesystem::create_directories(m_settings.output_folder + "screenshot/");
                        const auto file = fmt::format(
                            m_settings.output_folder + "screenshot/{}.png", time_to_string(std::time(nullptr)));
                        if (!m_window->screenshot(file)) { spdlog::warn("failed to take a screenshot: {}", file); }
                    } break;
                    default: break;
                    }
                },
                [&](const Connected<Joystick> &j) { m_joystickManager->add(j.source); },
                [&](const Disconnected<Joystick> &j) { m_joystickManager->remove(j.source); },
                [&](const Moved<JoystickAxis> &j) { m_joystickManager->update(j); },
                [&](const Pressed<JoystickButton> &j) { m_joystickManager->update(j); },
                [&](const Released<JoystickButton> &j) { m_joystickManager->update(j); },
                []([[maybe_unused]] const auto &) {}},
            event);

        if (timeElapsed) { this->tickOnce(std::get<TimeElapsed>(event)); }

        if (!timeElapsed || (timeElapsed && m_eventMode != EventMode::PAUSED)) m_game->onUpdate(m_world, event);
    }

    m_world.view<engine::Drawable>().each(engine::Drawable::dtor);

    m_game->onDestroy(m_world);

#ifndef NDEBUG
    nlohmann::json serialized(eventsProcessed);
    std::filesystem::create_directories(m_settings.output_folder + "logs/");
    std::ofstream f{m_settings.output_folder + "logs/recorded_events.json"};
    f << serialized;
#endif

    get().reset(nullptr);

    return 0;
}

auto engine::Core::tickOnce(const TimeElapsed &t) -> void
{
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed).count();

    if (m_eventMode != EventMode::PAUSED) {
        for (const auto &i : m_world.view<Lifetime>()) {
            auto &lifetime = m_world.get<Lifetime>(i);

            if (t.elapsed < lifetime.remaining_lifetime) {
                lifetime.remaining_lifetime -= std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed);
            } else {
                m_world.destroy(i);
            }
        }

        // should have only one entity cooldown
        m_world.view<entt::tag<"screenshake"_hs>, Cooldown>().each([this, elapsed](auto &, auto &cd) {
            if (!cd.is_in_cooldown) return;

            if (std::chrono::milliseconds{elapsed} < cd.remaining_cooldown) {
                cd.remaining_cooldown -= std::chrono::milliseconds{elapsed};
            } else {
                cd.remaining_cooldown = std::chrono::milliseconds(0);
                cd.is_in_cooldown = false;
                setScreenshake(false);
            }
        });

        // check if the spritesheet need to update the texture
        m_world.view<Spritesheet>().each([&elapsed](engine::Spritesheet &sprite) {
            if (!sprite.cooldown.is_in_cooldown) return;

            if (std::chrono::milliseconds{elapsed} < sprite.cooldown.remaining_cooldown) {
                sprite.cooldown.remaining_cooldown -= std::chrono::milliseconds{elapsed};
            } else {
                sprite.cooldown.remaining_cooldown = std::chrono::milliseconds(0);
                sprite.cooldown.is_in_cooldown = false;
            }
        });

        // update and reset the cooldown of the spritesheet
        for (auto &i : m_world.view<Spritesheet>()) {
            auto &sprite = m_world.get<Spritesheet>(i);
            if (sprite.cooldown.is_in_cooldown) continue;
            sprite.cooldown.is_in_cooldown = true;
            sprite.cooldown.remaining_cooldown = sprite.cooldown.cooldown;
            sprite.current_frame++;
            sprite.current_frame %=
                static_cast<std::uint16_t>(sprite.animations.at(sprite.current_animation).frames.size());

            auto &vbo_texture = m_world.get<VBOTexture>(i);
            auto &texture = *getCache<Texture>().handle(vbo_texture.id);

            DrawableFactory::fix_texture(
                m_world,
                i,
                m_settings.data_folder + sprite.animations.at(sprite.current_animation).file,
                {static_cast<float>(sprite.animations.at(sprite.current_animation).frames.at(sprite.current_frame).x)
                     / static_cast<float>(texture.width),
                 static_cast<float>(sprite.animations.at(sprite.current_animation).frames.at(sprite.current_frame).y)
                     / static_cast<float>(texture.height),
                 sprite.animations.at(sprite.current_animation).width / static_cast<float>(texture.width),
                 sprite.animations.at(sprite.current_animation).height / static_cast<float>(texture.height)});
        }

        m_world.view<d2::Velocity, d2::Acceleration>().each([](auto &vel, auto &acc) {
            vel.x += acc.x;
            vel.y += acc.y;

            // todo : add max velocity
        });

        // todo : exclude the d2::Hitbox on this system
        //            m_world.view<d3::Position, d2::Velocity>().each(
        //                [&elapsed](auto &pos, auto &vel) {
        //                    pos.x += vel.x * static_cast<decltype(vel.x)>(elapsed) / 1000.0;
        //                    pos.y += vel.y * static_cast<decltype(vel.y)>(elapsed) / 1000.0;
        //                });

        m_world.view<d3::Position, d2::Velocity>(entt::exclude<d2::HitboxSolid>).each([&elapsed](auto &pos, auto &vel) {
            pos.x += vel.x * static_cast<d2::Velocity::type>(elapsed) / 1000.0;
            pos.y += vel.y * static_cast<d2::Velocity::type>(elapsed) / 1000.0;
        });

        for (auto &moving : m_world.view<d3::Position, d2::Velocity, d2::HitboxSolid>()) {
            auto &moving_pos = m_world.get<d3::Position>(moving);
            auto &moving_vel = m_world.get<d2::Velocity>(moving);
            auto &moving_hitbox = m_world.get<d2::HitboxSolid>(moving);
            d2::Velocity actual_tick_velocity = moving_vel;

            const auto pred_pos = d3::Position{
                moving_pos.x + moving_vel.x * static_cast<d2::Velocity::type>(elapsed) / 1000.0,
                moving_pos.y + moving_vel.y * static_cast<d2::Velocity::type>(elapsed) / 1000.0,
                moving_pos.z};

            d3::Position other_pos;
            d2::HitboxSolid other_hitbox;

            for (auto &others : m_world.view<d3::Position, d2::HitboxSolid>()) {
                if (moving == others) continue;

                other_pos = m_world.get<d3::Position>(others);
                other_hitbox = m_world.get<d2::HitboxSolid>(others);

                if (d2::overlapped<d2::WITH_EDGE>(moving_hitbox, pred_pos, other_hitbox, other_pos)) {
                    auto xDiff = other_pos.x - pred_pos.x;
                    auto xLastDiff = other_pos.x - moving_pos.x;
                    auto xMinSpace = (moving_hitbox.width + other_hitbox.width) / 2;
                    if (std::abs(xDiff) < xMinSpace && std::abs(xLastDiff) >= xMinSpace) actual_tick_velocity.x = 0;

                    auto yDiff = other_pos.y - pred_pos.y;
                    auto yLastDiff = other_pos.y - moving_pos.y;
                    auto yMinSpace = (moving_hitbox.height + other_hitbox.height) / 2;
                    if (std::abs(yDiff) < yMinSpace && std::abs(yLastDiff) >= yMinSpace) actual_tick_velocity.y = 0;
                }
            }

            moving_pos.x += actual_tick_velocity.x * static_cast<d2::Velocity::type>(elapsed) * 0.001;
            moving_pos.y += actual_tick_velocity.y * static_cast<d2::Velocity::type>(elapsed) * 0.001;
        }
    }

    m_window->draw([&] {
        m_game->drawUserInterface(m_world);

#ifndef NDEBUG
        if (isShowingDebugInfo()) {
            ImGui::ShowDemoWindow();
            debugDrawJoystick();
            debugDrawDisplayOptions();
        }
#endif

        ImGui::Render();

        const auto background = m_game->getBackgroundColor();

        CALL_OPEN_GL(::glClearColor(background.r, background.g, background.b, 1.0f));
        CALL_OPEN_GL(::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        // todo : add rendering
        // texture and no color
        // no texture and no color

        static std::decay_t<decltype(elapsed)> tmp = 0; // note : elapsed time since the start of the app
        tmp += elapsed;

        m_shader_colored->use();
        m_shader_colored->setUniform<float>("time", static_cast<float>(tmp));
        m_world.view<Drawable, Color, d3::Position, d2::Scale>(entt::exclude<VBOTexture>)
            .each([this](auto entity, auto &drawable, [[maybe_unused]] auto &color, auto &pos, auto &scale) {
                auto *rotationComponent = m_world.try_get<d2::Rotation>(entity);
                auto rotation = rotationComponent ? static_cast<float>(rotationComponent->angle) : 0.f;

                auto model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3{pos.x, pos.y, pos.z});
                model = glm::rotate(model, rotation, glm::vec3(0.f, 0.f, 1.f));
                model = glm::scale(model, glm::vec3{scale.x, scale.y, 1.0f});
                m_shader_colored->setUniform("model", model);
                CALL_OPEN_GL(::glBindVertexArray(drawable.VAO));
                CALL_OPEN_GL(::glDrawElements(m_displayMode, 3 * drawable.triangle_count, GL_UNSIGNED_INT, 0));
            });

        m_shader_colored_textured->use();
        m_shader_colored_textured->setUniform<float>("time", static_cast<float>(tmp));
        m_world.view<Drawable, Color, VBOTexture, d3::Position, d2::Scale>().each(
            [this](auto entity, auto &drawable, [[maybe_unused]] auto &color, auto &texture, auto &pos, auto &scale) {
                auto *rotationComponent = m_world.try_get<d2::Rotation>(entity);
                auto rotation = rotationComponent ? static_cast<float>(rotationComponent->angle) : 0.f;

                auto model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3{pos.x, pos.y, pos.z});
                model = glm::rotate(model, rotation, glm::vec3(0.f, 0.f, 1.f));
                model = glm::scale(model, glm::vec3{scale.x, scale.y, 1.0f});
                m_shader_colored_textured->setUniform("model", model);
                m_shader_colored_textured->setUniform("mirrored", texture.mirrored);
                CALL_OPEN_GL(::glBindTexture(GL_TEXTURE_2D, getCache<Texture>().handle(texture.id)->id));
                CALL_OPEN_GL(::glBindVertexArray(drawable.VAO));
                CALL_OPEN_GL(::glDrawElements(m_displayMode, 3 * drawable.triangle_count, GL_UNSIGNED_INT, 0));
            });
    });
}

auto engine::Core::updateView(const glm::mat4 &view) -> void
{
    m_shader_colored->use();
    m_shader_colored->setUniform("viewProj", view);
    m_shader_colored_textured->use();
    m_shader_colored_textured->setUniform("viewProj", view);
}

auto engine::Core::setScreenshake(bool value, std::chrono::milliseconds delay) -> void
{
    m_shader_colored->use();
    m_shader_colored->setUniform<bool>("shake", value);
    m_shader_colored_textured->use();
    m_shader_colored_textured->setUniform<bool>("shake", value);

    if (value) {
        m_world.view<entt::tag<"screenshake"_hs>, Cooldown>().each([&delay](auto &, auto &cd) {
            cd.cooldown = delay;
            cd.remaining_cooldown = delay;
            cd.is_in_cooldown = true;
        });
    }
}

auto engine::Core::get() noexcept -> std::unique_ptr<Core> &
{
    static auto instance = std::make_unique<Core>(hidden_type{});
    return instance;
}

template<>
auto engine::Core::getCache() noexcept -> entt::resource_cache<Color> &
{
    return m_colors;
}

template<>
auto engine::Core::getCache() noexcept -> entt::resource_cache<VBOTexture> &
{
    return m_vbo_textures;
}

template<>
auto engine::Core::getCache() noexcept -> entt::resource_cache<Texture> &
{
    return m_textures;
}

auto engine::Core::loadOpenGL() -> void
{
    if (const auto err = ::glewInit(); err != GLEW_OK) {
        throw std::logic_error(
            fmt::format("Engine::Core GLEW An error occured '{}' 'code={}'", ::glewGetErrorString(err), err));
    }
}

auto engine::Core::getElapsedTime() noexcept -> std::chrono::nanoseconds
{
    static constexpr auto kMax = std::chrono::milliseconds(50);

    const auto nextTick = std::chrono::steady_clock::now();
    auto timeElapsed = nextTick - m_lastTick;

    if (timeElapsed > kMax)
        timeElapsed = std::chrono::nanoseconds(kMax);

    m_lastTick += timeElapsed;
    return timeElapsed;
}

auto engine::Core::getJoystick(int id) -> std::optional<Joystick *const> { return m_joystickManager->get(id); }

#ifndef NDEBUG

// todo : what happen when more than 1 joystick ?
auto engine::Core::debugDrawJoystick() -> void
{
    ImGui::Begin("Joystick");
    m_joystickManager->each([](const Joystick &joy) {
        ImGui::BeginChild("Scrolling");
        for (std::uint32_t n = 0; n < Joystick::BUTTONS_MAX; n++) {
            helper::ImGui::Text(
                "{} = {}", magic_enum::enum_name(magic_enum::enum_cast<Joystick::Buttons>(n).value()).data(), joy.buttons[n]);
        }
        ImGui::EndChild();

        ImGui::BeginChild("Scrolling");
        for (std::uint32_t n = 0; n < Joystick::AXES_MAX; n++) {
            helper::ImGui::Text(
                "{} = {}",
                magic_enum::enum_name(magic_enum::enum_cast<Joystick::Axis>(n).value()).data(),
                static_cast<double>(joy.axes[n]));
        }
        ImGui::EndChild();
    });
    ImGui::End();
}

// todo : add this : https://learnopengl.com/Advanced-OpenGL/Face-culling

auto engine::Core::debugDrawDisplayOptions() -> void
{
    enum DisplayMode : std::uint32_t {
        POINTS = GL_POINTS,
        LINE_STRIP = GL_LINE_STRIP,
        LINE_LOOP = GL_LINE_LOOP,
        LINES = GL_LINES,
        LINE_STRIP_ADJACENCY = GL_LINE_STRIP_ADJACENCY,
        LINES_ADJACENCY = GL_LINES_ADJACENCY,
        TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
        TRIANGLE_FAN = GL_TRIANGLE_FAN,
        TRIANGLES = GL_TRIANGLES,
        TRIANGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY,
        TRIANGLES_ADJACENCY = GL_TRIANGLES_ADJACENCY,
        PATCHES = GL_PATCHES,
    };

    static constexpr auto display_mode = std::to_array({
        POINTS,
        LINE_STRIP,
        LINE_LOOP,
        LINES,
        LINE_STRIP_ADJACENCY,
        LINES_ADJACENCY,
        TRIANGLE_STRIP,
        TRIANGLE_FAN,
        TRIANGLES,
        TRIANGLE_STRIP_ADJACENCY,
        TRIANGLES_ADJACENCY,
        PATCHES,
    });

    ImGui::Begin("Display Options");
    if (ImGui::BeginCombo("##combo", "Display Mode")) {
        for (auto n = 0ul; n < display_mode.size(); n++) {
            bool is_selected = (m_displayMode == static_cast<std::uint32_t>(display_mode.at(n)));
            if (ImGui::Selectable(magic_enum::enum_name<DisplayMode>(display_mode.at(n)).data(), is_selected))
                m_displayMode = static_cast<std::uint32_t>(display_mode.at(n));
            if (is_selected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::End();
}

#endif
