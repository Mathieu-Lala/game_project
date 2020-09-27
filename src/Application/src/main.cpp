#include <fstream>
#include <streambuf>

#include <Engine/Core.hpp>

#include <Engine/Shader.hpp>

#include <Engine/component/Drawable.hpp>

struct Triangle {
    static auto create(float x, float y) -> engine::Drawable
    {
        // clang-format off
        const float VERTICES[] = {
            // positions                                    // colors
             0.5f + x, -0.5f + y, 0.0f,   1.0f, 0.0f, 0.0f,  // bottom right
            -0.5f + x, -0.5f + y, 0.0f,   0.0f, 1.0f, 0.0f,  // bottom left
             0.0f + x,  0.5f + y, 0.0f,   0.0f, 0.0f, 1.0f   // top
        };
        // clang-format on

        unsigned int VBO;
        unsigned int VAO;

        ::glGenVertexArrays(1, &VAO);
        ::glGenBuffers(1, &VBO);

        ::glBindVertexArray(VAO);

        ::glBindBuffer(GL_ARRAY_BUFFER, VBO);
        ::glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

        // position attribute
        ::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void *>(0));
        ::glEnableVertexAttribArray(0);

        // color attribute
        ::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
        ::glEnableVertexAttribArray(1);

        return {VBO, VAO};
    }
};

auto open(const std::string_view file)
{
    std::ifstream t(file.data());
    return std::string(std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>());
}

struct ThePurge : public engine::Game {
    ThePurge() : m_shader{open("./data/shaders/vertex.glsl"), open("./data/shaders/fragment.glsl")} {}

    auto onCreate(entt::registry &world) -> void final
    {
        for (int i = 0; i != 10; i++) {
            auto e = world.create();

            constexpr auto max = static_cast<float>(std::numeric_limits<int>::max());

            world.emplace<engine::Drawable>(
                e, Triangle::create(static_cast<float>(std::rand()) / max - 0.5f, static_cast<float>(std::rand()) / max - 0.5f));

            m_triangles.emplace_back(e);
        }
    }

    auto onUpdate(entt::registry &world) -> void final
    {
        world.view<engine::Drawable>().each([&](engine::Drawable &drawable) {
            m_shader.use();

            ::glBindVertexArray(drawable.VAO);
            ::glDrawArrays(GL_TRIANGLES, 0, 3);
        });
    }

    auto onDestroy(entt::registry &world) -> void final
    {
        world.view<engine::Drawable>().each([&](engine::Drawable &drawable) {
            ::glDeleteVertexArrays(1, &drawable.VAO);
            ::glDeleteBuffers(1, &drawable.VBO);
        });
    }

    engine::Shader m_shader;

    std::vector<entt::entity> m_triangles;
};

int main(int ac, char **av)
{
    engine::Core::Holder holder;

    holder.instance->window(glm::ivec2{400, 400}, "The PURGE");
    holder.instance->game<ThePurge>();

    return holder.instance->main(ac, av);
}
