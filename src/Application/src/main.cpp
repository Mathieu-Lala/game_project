#include <Engine/Core.hpp>

int main()
{
    engine::Core::Holder holder;

    auto &window = holder.instance->window(glm::ivec2{ 200, 200 }, "Hello world #1");

    bool show_demo_window = true;
    while (window->isOpen()) {

        window->draw([&] {

            if (show_demo_window)
                ImGui::ShowDemoWindow(&show_demo_window);

            ImGui::Render();

            glClearColor(0.45f, 0.55f,  0.60f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);

        });

        ::glfwPollEvents();
        if (::glfwGetKey(window->m_handle, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            window->close();

    }

    // otherwise the singleton will be destroy after the main -> dead signal
    holder.instance.reset(nullptr);

    return 0;
}
