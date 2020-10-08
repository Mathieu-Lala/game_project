#include <fstream>
#include <streambuf>
#include <entt/entt.hpp>
#include <iostream>

#include <Engine/Core.hpp>

#include <Engine/Shader.hpp>

#include <Engine/component/Drawable.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Terrain/Floor.hpp"
#include <Declaration.hpp>
#include <Competences/FarmerCompetences.hpp>

#include <glm/gtx/string_cast.hpp>

auto get() -> engine::Drawable
{
    float VERTICES[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
        0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0,
        1,
        3, // first triangle
        1,
        2,
        3 // second triangle
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void *>(0));
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
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
    ThePurge() : m_shader{open(DATA_DIR "/shaders/vertex.glsl"), open(DATA_DIR "/shaders/fragment.glsl")} {}

    auto onCreate(entt::registry &world) -> void final
    {
        // constexpr auto max = static_cast<float>(std::numeric_limits<int>::max());

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

        // world.emplace<engine::Drawable>(world.create(), get());


        getCamera().setViewport(-0.8, 0.8, 0.45, -0.45); // simple 16:9 test viewport
        getCamera().setCenter({0.05, 0.05}); // test tile center

        _floor = std::make_unique<Floor>(world, glm::vec2{0, 0});
    }

    auto onUpdate(entt::registry &world) -> void final
    {       
        world.view<engine::Drawable>().each([&](engine::Drawable &drawable) {
            drawable.shader->uploadUniformMat4("u_ViewProjection", getCamera().getViewProjMatrix());
            
            drawable.shader->use();

            ::glBindVertexArray(drawable.VAO);
            ::glDrawElements(GL_TRIANGLES, 3 * drawable.triangle_count, GL_UNSIGNED_INT, 0);
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

    std::unique_ptr<Floor> _floor;
};

void foo(int a, char b)
{ std::cout << a << b << std::endl; }

struct listener {
    void bar(const int &a, char b)
    { 
        std::cout << a << b << std::endl;
    }
};

class boop {
public:
    void printTest(const int &a, char b) { 
        std::cout << a << " || " << b << std::endl;
    }
};

class Controller {
public:
    void printA(const std::string &str) { std::cout << str << std::endl; };
    void printB(const std::string &str) { std::cout << str << std::endl; };
    void printX(const std::string &str) { std::cout << str << std::endl; };
    void printY(const std::string &str) { std::cout << str << std::endl; };
};

int main(int ac, char **av)
{
    engine::Core::Holder holder;

    //holder.instance->window(glm::ivec2{400, 400}, "The PURGE");
    //holder.instance->game<ThePurge>();

    // ...
    //entt::sigh<void(int, char)> signal;
    //entt::sink sink{signal};
    //listener instance;
    //boop test;

    //sink.connect<&foo>();
    //sink.connect<&listener::bar>(instance);
    //sink.connect<&boop::printTest>(test);

    std::string buffer{};

    Controller controller;
    FarmerCompetences farmer;

    entt::sigh<void(void)> actionBottom;
    entt::sigh<void(void)> actionRight;
    entt::sigh<void(const std::string &)> actionLeft;
    entt::sigh<void(const std::string &)> actionTop;

    entt::sink sinkB(actionBottom);
    entt::sink sinkR(actionRight);
    entt::sink sinkL(actionLeft);
    entt::sink sinkT(actionTop);

    sinkB.connect<&FarmerCompetences::activateSkill>(farmer);
    sinkR.connect<&FarmerCompetences::displayInfos>(farmer);
    sinkL.connect<&Controller::printX>(controller);
    sinkT.connect<&Controller::printY>(controller);

    std::cout;
    while (std::getline(std::cin, buffer)) {
        std::cout << "Input -> " << buffer << std::endl;
        if (buffer == "A" || buffer == "ACTION_BOTTOM") {
            actionBottom.publish();
        } else if (buffer == "B" || buffer == "ACTION_RIGHT") {
            actionRight.publish();
        } else if (buffer == "X" || buffer == "ACTION_LEFT") {
            actionLeft.publish("X is Pressed !");
        } else if (buffer == "Y" || buffer == "ACTION_RIGHT") {
            actionTop.publish("Y is Pressed !");
        } else if (buffer == "exit") break;
    }

    //// ...
    //signal.publish(42, 'c');

    //// disconnects a free function
    //sink.disconnect<&foo>();

    //// disconnect a member function of an instance
    //sink.disconnect<&listener::bar>(instance);

    //// disconnect all member functions of an instance, if any
    //sink.disconnect(instance);

    //// discards all listeners at once
    //sink.disconnect();

    return holder.instance->main(ac, av);
}
