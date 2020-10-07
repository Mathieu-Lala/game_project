#include <sstream>

#include <CLI/CLI.hpp>

#include <stb_image.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <Engine/Core.hpp>
#include <Engine/Shader.hpp>
#include <Engine/component/Drawable.hpp>

#include "Declaration.hpp"
#include "level/LevelTilemapBuilder.hpp"
#include "level/TileFactory.hpp"
#include "level/MapGenerator.hpp"

class ThePurge : public engine::Game {

    glm::vec4 screen_pos = { 0, 89, 0, 50 };

    auto onCreate(entt::registry &world) -> void final
    {
        generateFloor(world, {}, static_cast<std::uint32_t>(::time(nullptr)));

        m_camera.setViewport(screen_pos.x, screen_pos.y, screen_pos.z, screen_pos.w);
        m_camera.setCenter({0, 0});

        TileFactory::getShader()->uploadUniformMat4("u_ViewProjection", m_camera.getViewProjMatrix());
    }

    auto onDestroy(entt::registry &) -> void final
    {
    }

    auto onUpdate(entt::registry &world, const engine::Event &e) -> void final
    {
        std::visit(engine::overloaded{
            [&](const engine::Pressed<engine::Key> &key) {
                spdlog::info("key pressed {}", key.source.key);

                // note : this does not work as expected ..
                switch (key.source.key) {
                case GLFW_KEY_UP: screen_pos += glm::vec4(-10, 10, 0, 0); break;
                case GLFW_KEY_RIGHT: screen_pos += glm::vec4(10, -10, 0, 0); break;
                case GLFW_KEY_DOWN: screen_pos += glm::vec4(0, 0, 10, -10); break;
                case GLFW_KEY_LEFT: screen_pos += glm::vec4(0, 0, -10, 10); break;
                default: break;
                }
                m_camera.setViewport(screen_pos.x, screen_pos.y, screen_pos.z, screen_pos.w);
                TileFactory::getShader()->uploadUniformMat4("u_ViewProjection", m_camera.getViewProjMatrix());

            },
            [&](const engine::TimeElapsed &t) { onTick(world, t.elapsed); },
            [](auto) { },
        }, e);
    }

    bool show_demo_window = true;

    auto drawUserInterface() -> void final
    {
        if (show_demo_window) { ImGui::ShowDemoWindow(&show_demo_window); }
    }

private:

    engine::Camera m_camera;

    auto onTick(entt::registry &, const std::chrono::steady_clock::duration &) -> void
    {
    }

};

static constexpr auto NAME =
R"(ThePURGE 0.1.8)";

static constexpr auto VERSION =
"ThePURGE 0.1.8"
#ifndef NDEBUG
    " - Debug build"
#else
    " - Release build"
#endif
;

struct Options {

    enum Value {
        FULLSCREEN,
        REPLAY_PATH,
        OPTION_MAX
    };

    CLI::App app;

    std::array<CLI::Option *, OPTION_MAX> options;

    bool fullscreen = true;

    std::string replay_path;

    Options(int argc, char **argv) :
        app(NAME, argv[0])
    {
        app.add_flag("--version", [](auto) -> void {
            std::cout << VERSION; exit(0); }, "Print the version number and exit.");

        options[FULLSCREEN] = app.add_option("--fullscreen", fullscreen, "Launch the window in fullscreen mode.", true);
        options[REPLAY_PATH] = app.add_option("--replay_path", replay_path, "Path of the events to replay.");

        if (const auto res = [&]() -> std::optional<int> { CLI11_PARSE(app, argc, argv); return {}; }(); res.has_value()) { throw res.value_or(0); }
    }

    void dump() const
    {
        spdlog::warn("Working on file: {}, direct count: {}, opt count: {}",
            fullscreen, app.count("--fullscreen"), options[FULLSCREEN]->count());
    }
};

int main(int argc, char **argv) try
{
    Options opt{argc, argv};

    opt.dump();

    // todo :
    auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
    logger->info("logger created");


    auto holder = engine::Core::Holder::init();

    std::uint16_t windowProperty = engine::Window::Property::DEFAULT;
    if (opt.fullscreen)
        windowProperty |= engine::Window::Property::FULLSCREEN;

    holder.instance->window(glm::ivec2{400, 400}, VERSION, windowProperty);
    holder.instance->game<ThePurge>();

#ifndef NDEBUG
    if (opt.options[Options::REPLAY_PATH]->count())
        holder.instance->setPendingEventsFromFile(opt.replay_path);
#endif

    return holder.instance->main();
}
catch (const std::exception &e)
{
    spdlog::error("Caught exception at main level: {}", e.what());
}
catch (int code)
{
    return code;
}
