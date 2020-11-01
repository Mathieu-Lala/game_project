#include <fstream>
#include <functional>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <magic_enum.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include "Engine/helpers/overloaded.hpp"
#include "Engine/component/Drawable.hpp"
#include "Engine/component/Position.hpp"
#include "Engine/component/Scale.hpp"
#include "Engine/component/Velocity.hpp"
#include "Engine/component/Acceleration.hpp"
#include "Engine/component/Hitbox.hpp"
#include "Engine/component/Color.hpp"
#include "Engine/component/Spritesheet.hpp"

#include "Engine/Event/Event.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Window.hpp"
#include "Engine/Event/JoystickManager.hpp"
#include "Engine/Core.hpp"

#include "Engine/helpers/DrawableFactory.hpp"
#include "Engine/helpers/ImGui.hpp"

#include "Engine/Graphics/third_party.hpp" // note : only for DisplayMode

// todo : remove me
#include "Engine/../../../Application/include/Declaration.hpp"

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

    spdlog::info("Engine::Core instanciated");
    if (::glfwInit() == GLFW_FALSE) { throw std::logic_error(fmt::format("Engine::Core initialization failed")); }

    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    ::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    ::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    ::glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    spdlog::info("Engine::Core GLFW version: '{}'\n", ::glfwGetVersionString());

    IMGUI_CHECKVERSION();

    m_joystickManager = std::make_unique<JoystickManager>();

    //    ::stbi_set_flip_vertically_on_load(true);
}

engine::Core::~Core()
{
    ::glfwTerminate();

    // otherwise we have a final error message at the end
    ::glfwSetErrorCallback(nullptr);

    spdlog::info("Engine::Core destroyed");
}

auto engine::Core::setPendingEvents(std::vector<Event> &&events) -> void
{
    m_eventMode = PLAYBACK;
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
    case RECORD: {
        m_joystickManager->poll();

        // 1. poll the window event
        // 2. poll the joysticks event
        // 3. send elapsed time
        return m_window->getNextEvent().value_or(m_joystickManager->getNextEvent().value_or(TimeElapsed{getElapsedTime()}));

    } break;
    case PLAYBACK: {
        if (m_eventsPlayback.empty()) {
            spdlog::warn("Engine::Window switching to record mode");
            m_eventMode = RECORD;
            return TimeElapsed{getElapsedTime()};
        }
        auto event = m_eventsPlayback.front();
        m_eventsPlayback.erase(m_eventsPlayback.begin());

        // apply the event to the engine
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
                // todo : add fullscreen
            },
            event);
        return event;

    } break;
    default: std::abort();
    }
}

auto engine::Core::main() -> int
{
    if (m_window == nullptr || m_game == nullptr) { return 1; }

    m_shader_colored.reset(
        new Shader{Shader::fromFile(DATA_DIR "shaders/colored.vert.glsl", DATA_DIR "shaders/colored.frag.glsl")});

    m_shader_colored_textured.reset(new Shader{
        Shader::fromFile(DATA_DIR "shaders/colored_textured.vert.glsl", DATA_DIR "shaders/colored_textured.frag.glsl")});

    // todo : add max size buffer ?
    std::vector<Event> eventsProcessed{TimeElapsed{}};

    while (isRunning()) { // note Core::isRunning instead ?

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
        bool keyPressed = false;

        // note : engine related
        std::visit(
            overloaded{
                [&]([[maybe_unused]] const OpenWindow &) {
                    m_lastTick = std::chrono::steady_clock::now();
                    ::glViewport(0, 0, static_cast<int>(m_window->getSize().x), static_cast<int>(m_window->getSize().y));
                },
                [&]([[maybe_unused]] const CloseWindow &) {
                    m_window->close();
                    this->close();
                },
                [&](const ResizeWindow &e) { ::glViewport(0, 0, e.width, e.height); }, // todo : move this in camera ? or window ?
                [&]([[maybe_unused]] const TimeElapsed &) { timeElapsed = true; },
                [&]([[maybe_unused]] const Pressed<Key> &) { keyPressed = true; },
                [&](const Connected<Joystick> &j) { m_joystickManager->add(j.source); },
                [&](const Disconnected<Joystick> &j) { m_joystickManager->remove(j.source); },
                [&](const Moved<JoystickAxis> &j) { m_joystickManager->update(j); },
                [&](const Pressed<JoystickButton> &j) { m_joystickManager->update(j); },
                [&](const Released<JoystickButton> &j) { m_joystickManager->update(j); },
                []([[maybe_unused]] const auto &) {}},
            event);

        if (keyPressed) {
            // todo : abstract glfw keyboard
            const auto keyEvent = std::get<Pressed<Key>>(event);
            if (keyEvent.source.key == GLFW_KEY_ESCAPE) this->close();
            if (keyEvent.source.key == GLFW_KEY_F11) m_window->setFullscreen(!m_window->isFullscreen());
        }

        // note : should note draw at every frame = heavy
        if (timeElapsed) {
            const auto t = std::get<TimeElapsed>(event);
            const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed).count();

            // check if the spritesheet need to update the texture
            m_world.view<Spritesheet>().each([&elapsed](engine::Spritesheet &sprite) {
                if (!sprite.speed.is_in_cooldown) return;

                if (std::chrono::milliseconds{elapsed} < sprite.speed.remaining_cooldown) {
                    sprite.speed.remaining_cooldown -= std::chrono::milliseconds{elapsed};
                } else {
                    sprite.speed.is_in_cooldown = false;
                }
            });

            // update and reset the cooldown of the spritesheet
            for (auto &i : m_world.view<Spritesheet>()) {
                auto &sprite = m_world.get<Spritesheet>(i);
                if (sprite.speed.is_in_cooldown) continue;
                sprite.speed.is_in_cooldown = true;
                sprite.speed.remaining_cooldown = sprite.speed.cooldown;
                sprite.current_frame++;
                sprite.current_frame %= static_cast<std::uint16_t>(sprite.frames.size());

                auto &texture = m_world.get<Texture>(i);

                DrawableFactory::fix_texture(
                    m_world,
                    i,
                    std::string(DATA_DIR) + sprite.file,
                    {static_cast<float>(sprite.frames[sprite.current_frame].x) / static_cast<float>(texture.width),
                     static_cast<float>(sprite.frames[sprite.current_frame].y) / static_cast<float>(texture.height),
                     sprite.width / static_cast<float>(texture.width),
                     sprite.height / static_cast<float>(texture.height)});
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

            for (auto &moving : m_world.view<d3::Position, d2::Velocity, d2::HitboxSolid>()) {
                auto &moving_pos = m_world.get<d3::Position>(moving);
                auto &moving_vel = m_world.get<d2::Velocity>(moving);
                auto &moving_hitbox = m_world.get<d2::HitboxSolid>(moving);

                const auto new_pos = d3::Position{
                    moving_pos.x + moving_vel.x * static_cast<d2::Velocity::type>(elapsed) / 1000.0,
                    moving_pos.y + moving_vel.y * static_cast<d2::Velocity::type>(elapsed) / 1000.0,
                    moving_pos.z};

                bool collide = false;

                d3::Position others_pos;
                d2::HitboxSolid others_hitbox;

                for (auto &others : m_world.view<d3::Position, d2::HitboxSolid>()) {
                    if (moving == others) continue;

                    others_pos = m_world.get<d3::Position>(others);
                    others_hitbox = m_world.get<d2::HitboxSolid>(others);

                    if (d2::overlapped<d2::WITH_EDGE>(moving_hitbox, new_pos, others_hitbox, others_pos)) {
                        // spdlog::warn("{} and {} are colliding !", moving, others);
                        collide = true;
                        break;
                    }
                }

                if (!collide) {
                    moving_pos = new_pos;
                } else {
                    moving_vel = {0.0f, 0.0f};
#ifdef COLLISION_EXPERIMENTAL
                    if (!d2::overlapped<d2::WITHOUT_EDGE>(
                            moving_hitbox, d3::Position{new_pos.x, moving_pos.x, 0}, others_hitbox, others_pos)) {
                        moving_vel.y = 0;
                    } else if (!d2::overlapped<d2::WITHOUT_EDGE>(
                                   moving_hitbox, d3::Position{moving_pos.y, new_pos.y, 0}, others_hitbox, others_pos)) {
                        moving_vel.x = 0;
                    }
#endif
                }
            }

#ifdef MODE_EPILEPTIC // change the color at every frame
            for (const auto &i : m_world.view<Drawable, Color>()) {
                auto &color = m_world.get<Color>(i);
                const auto r = std::clamp(Color::r(color) - 0.01f, 0.0f, 1.0f);
                const auto g = std::clamp(Color::g(color) - 0.01f, 0.0f, 1.0f);
                const auto b = std::clamp(Color::b(color) - 0.01f, 0.0f, 1.0f);
                DrawableFactory::fix_color(m_world, i, {r ? r : 1.0f, g ? g : 1.0f, b ? b : 1.0f});
            }
#endif

            m_window->draw([&] {
                m_game->drawUserInterface(m_world);

#ifndef NDEBUG
                debugDrawJoystick();
                debugDrawDisplayOptions();
#endif

                ImGui::Render();

                const auto background = m_game->getBackgroundColor();

                ::glClearColor(background.r, background.g, background.b, 1.00f);
                ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // todo : add rendering
                // texture and no color
                // no texture and no color

                m_shader_colored->use();
                m_world.view<Drawable, Color, d3::Position, d2::Scale>(entt::exclude<Texture>)
                    .each([this](auto &drawable, [[maybe_unused]] auto &color, auto &pos, auto &scale) {
                        auto model = glm::dmat4(1.0);
                        model = glm::translate(model, glm::dvec3{pos.x, pos.y, pos.z});
                        model = glm::scale(model, glm::dvec3{scale.x, scale.y, 1.0});
                        m_shader_colored->uploadUniformMat4("model", model);

                        ::glBindVertexArray(drawable.VAO);

                        ::glDrawElements(m_displayMode, 3 * drawable.triangle_count, GL_UNSIGNED_INT, 0);
                    });

                m_shader_colored_textured->use();
                m_world.view<Drawable, Color, Texture, d3::Position, d2::Scale>().each(
                    [this](auto &drawable, [[maybe_unused]] auto &color, auto &texture, auto &pos, auto &scale) {
                        auto model = glm::dmat4(1.0);
                        model = glm::translate(model, glm::dvec3{pos.x, pos.y, pos.z});
                        model = glm::scale(model, glm::dvec3{scale.x, scale.y, 1.0});
                        m_shader_colored_textured->uploadUniformMat4("model", model);

                        ::glBindTexture(GL_TEXTURE_2D, texture.texture);
                        ::glBindVertexArray(drawable.VAO);

                        ::glDrawElements(m_displayMode, 3 * drawable.triangle_count, GL_UNSIGNED_INT, 0);
                    });
            });
        }

        m_game->onUpdate(m_world, event);
    }

    m_world.view<engine::Drawable>().each(engine::Drawable::dtor);

    m_game->onDestroy(m_world);

#ifndef NDEBUG
    nlohmann::json serialized(eventsProcessed);
    std::ofstream f{"recorded_events.json"};
    f << serialized;
#endif

    // otherwise the singleton will be destroy after the main -> dead signal
    get().reset(nullptr);

    return 0;
}

auto engine::Core::updateView(const glm::mat4 &view) -> void
{
    m_shader_colored->uploadUniformMat4("viewProj", view);
    m_shader_colored_textured->uploadUniformMat4("viewProj", view);
}

auto engine::Core::get() noexcept -> std::unique_ptr<Core> &
{
    static auto instance = std::make_unique<Core>(hidden_type{});
    return instance;
}

auto engine::Core::loadOpenGL() -> void
{
    if (const auto err = ::glewInit(); err != GLEW_OK) {
        throw std::logic_error(
            fmt::format("Engine::Core GLEW An error occured '{}' 'code={}'", ::glewGetErrorString(err), err));
    }
}

auto engine::Core::getElapsedTime() -> std::chrono::nanoseconds
{
    const auto nextTick = std::chrono::steady_clock::now();
    const auto timeElapsed = nextTick - m_lastTick;
    m_lastTick = nextTick;
    return timeElapsed;
}

auto engine::Core::getJoystick(int id) -> std::optional<Joystick *const>
{
    return m_joystickManager->get(id);
}

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
                double{joy.axes[n]});
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
