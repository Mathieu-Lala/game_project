#include <fstream>

#include <Engine/Core.hpp>
#include <Engine/Shader.hpp>

constexpr auto VERTEX_SHADER =
R"GLSL(#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
}
)GLSL";

constexpr auto FRAGMENT_SHADER =
R"GLSL(#version 330 core
out vec4 FragColor;

in vec3 ourColor;

void main()
{
    FragColor = vec4(ourColor, 1.0f);
}
)GLSL";

constexpr
float VERTICES[] = {
    // positions          // colors
     0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // bottom left
     0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f   // top
};

int main(int ac, char **av)
{
    engine::Core::Holder holder;

    auto &window = holder.instance->window(glm::ivec2{ 400, 400 }, "Hello world #1");

    if (ac == 2) {
        std::ifstream ifs(av[1]);
        const auto j = nlohmann::json::parse(ifs);
        window->setPendingEvents(j.get<std::vector<engine::Event>>());
    }
    spdlog::warn("Engine::Window is in {} mode", ac == 2 ? "playback" : "record");

    std::vector<engine::Event> eventsProcessed{ engine::TimeElapsed{} };



    engine::Shader shader{ VERTEX_SHADER, FRAGMENT_SHADER };

    unsigned int VBO;
    unsigned int VAO;
    ::glGenVertexArrays(1, &VAO);
    ::glGenBuffers(1, &VBO);

    ::glBindVertexArray(VAO);

    ::glBindBuffer(GL_ARRAY_BUFFER, VBO);
    ::glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

    // position attribute
    ::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    ::glEnableVertexAttribArray(0);
    // color attribute
    ::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
    ::glEnableVertexAttribArray(1);



    bool show_demo_window = true;
    while (window->isOpen()) {

        const auto event = window->getNextEvent();
        // note : first frame of draw is 'skipped' because of the sleep in record mode

        std::visit(engine::overloaded{
            [](engine::TimeElapsed &prev, const engine::TimeElapsed &next) { prev.elapsed += next.elapsed; },
            [&](const auto & /*prev*/, const std::monostate &) {},
            [&](const auto & /*prev*/, const auto &next) {
                eventsProcessed.push_back(next); // event in playback mode will be saved twice = bad !
            } },
            eventsProcessed.back(),
            event);

        // todo : update event context with joystick/keyboard/mouse
        bool timeElapsed = false;
        bool keyPressed = false;

        // note : user defined function ? or engine related ?
        std::visit(engine::overloaded{
            [](const engine::OpenWindow &) { /* call on open */ },
            [&](const engine::CloseWindow &) { /* call on close */ window->close(); },
            [&](const engine::ResizeWindow &e) { ::glViewport(0, 0, e.width, e.height); },
            [&](const engine::TimeElapsed &) { timeElapsed = true; },
            [&](const engine::Pressed<engine::Key> &) { keyPressed = true; },
            [&](const std::monostate &) {},
            [&](const auto &) {} },
            event);

        if (keyPressed) {
            const auto keyEvent = std::get<engine::Pressed<engine::Key>>(event);
            if (keyEvent.source.key == GLFW_KEY_ESCAPE) // todo : abstract glfw keyboard
                window->close();
        }

        if (!timeElapsed) continue;

        window->draw([&] {

            if (show_demo_window)
                ImGui::ShowDemoWindow(&show_demo_window);

            ImGui::Render();

            ::glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
            ::glClear(GL_COLOR_BUFFER_BIT);

            shader.use();
            ::glBindVertexArray(VAO);
            ::glDrawArrays(GL_TRIANGLES, 0, 3);

        });

    }

    ::glDeleteVertexArrays(1, &VAO);
    ::glDeleteBuffers(1, &VBO);

    for (const auto &event : eventsProcessed) {
        std::visit(engine::overloaded{
            [](const auto &event_obj) { spdlog::trace("Event: {}", event_obj.name); },
            [](const std::monostate &) { spdlog::info("monorail"); } },
            event);
    }

    nlohmann::json serialized(eventsProcessed);
    std::ofstream f{ "recorded_events.json" };
    f << serialized;

    // otherwise the singleton will be destroy after the main -> dead signal
    holder.instance.reset(nullptr);

    return EXIT_SUCCESS;
}
