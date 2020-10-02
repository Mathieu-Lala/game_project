#include <fstream>
#include <streambuf>
#include <sstream>

#include <stb_image.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <Engine/Core.hpp>
#include <Engine/Shader.hpp>

#include <Engine/component/Drawable.hpp>


auto get() -> engine::Drawable
{
    float VERTICES[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(0));
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    int width;
    int height;
    int nrChannels;
    auto data = stbi_load("./data/textures/image.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        spdlog::warn("Failed to load texture");
    }
    stbi_image_free(data);

    return {VBO, VAO, EBO};
}

auto open(const std::string_view file)
{
    std::ifstream t(file.data());
    return std::string(std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>());
}

struct ThePurge : public engine::Game {
    ThePurge() : m_shader{open("./data/shaders/vertex.glsl"), open("./data/shaders/fragment.glsl")} {}

    auto onCreate(entt::registry &world) -> void final
    {
        //constexpr auto max = static_cast<float>(std::numeric_limits<int>::max());

        /*
        for (int i = 0; i != 10; i++) {
            auto e = world.create();

            world.emplace<engine::Drawable>(
                e, Triangle::create(static_cast<float>(std::rand()) / max - 0.5f, static_cast<float>(std::rand()) / max - 0.5f));
        }
        */
/*
        for (int i = 0; i != 10; i++) {
            auto e = world.create();

            world.emplace<engine::Drawable>(
                e, Rectangle::create(static_cast<float>(std::rand()) / max - 0.5f, static_cast<float>(std::rand()) / max - 0.5f));
        }
*/

        world.emplace<engine::Drawable>(world.create(), get());
    }

    auto onUpdate(entt::registry &world) -> void final
    {
        world.view<engine::Drawable>().each([&](engine::Drawable &drawable) {
            m_shader.use();

            ::glBindVertexArray(drawable.VAO);
            ::glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        });
    }

    auto onDestroy(entt::registry &world) -> void final
    {
        world.view<engine::Drawable>().each([&](engine::Drawable &drawable) {
            ::glDeleteVertexArrays(1, &drawable.VAO);
            ::glDeleteBuffers(1, &drawable.VBO);
            ::glDeleteBuffers(1, &drawable.EBO);
        });
    }

    engine::Shader m_shader;
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
