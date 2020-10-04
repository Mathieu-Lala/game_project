#include <sstream>

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
        generateFloor(world, {}, ::time(nullptr));

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

    auto onTick(entt::registry &world, const std::chrono::steady_clock::duration &) -> void
    {
        world.view<engine::Drawable>().each([&](engine::Drawable &drawable) {
            ::glBindVertexArray(drawable.VAO);
            ::glDrawElements(GL_TRIANGLES, 3 * drawable.triangle_count, GL_UNSIGNED_INT, 0);
        });
    }

};

static constexpr auto VERSION =
R"(ThePURGE 0.1.8)";

static constexpr auto USAGE =
R"(ThePURGE v0.1.8
    Usage:
        app (-h | --help)
        app --version
        app [-f | --fullscreen] [--play path]

    Options:
        -h --help       Show this message.
        --version       Show version.
        -f|--fullscreen Launch in fullscreen mode.
        --play <path>   Path of the events to playback.
)";

int main(int argc, char **argv) try
{
    const auto args = docopt::docopt(USAGE, { argv + 1, argv + argc }, true, VERSION);

#ifndef NDEBUG
    for (const auto &[flag, value] : args) {
        std::stringstream ss;
        ss << value;
        spdlog::info("Application launched with args[{}] = {}", flag, ss.str());
    }

    spdlog::set_level(spdlog::level::level_enum::trace);
#else

#endif

    // todo :
    auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
    logger->info("logger created");


    auto holder = engine::Core::Holder::init();

    std::uint16_t windowProperty = engine::Window::Property::DEFAULT;
    if (args.at("-f").asBool() || args.at("--fullscreen").asBool())
        windowProperty |= engine::Window::Property::FULLSCREEN;

    holder.instance->window(glm::ivec2{400, 400}, "The PURGE", windowProperty);
    holder.instance->game<ThePurge>();

    return holder.instance->main(args);
}
catch (const std::exception &e)
{
    spdlog::error("Caught exception at main level: {}", e.what());
}
