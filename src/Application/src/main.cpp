#include <Engine/Core.hpp>

int main()
{
    engine::Core::Holder holder;

    auto &window = holder.instance->window(glm::ivec2{ 400, 400 }, "Hello world #1");

    std::vector<engine::Event> eventsProcessed{ engine::TimeElapsed{} };

    bool show_demo_window = true;
    while (window->isOpen()) {

        const auto event = window->getNextEvent();
        std::visit(engine::overloaded{
            [](engine::TimeElapsed &prev, const engine::TimeElapsed &next) { prev.elapsed += next.elapsed; },
            [&](const auto & /*prev*/, const std::monostate &) {},
            [&](const auto & /*prev*/, const auto &next) { eventsProcessed.push_back(next); } },
            eventsProcessed.back(),
            event);

        // todo : update event context with joystick/keyboard/mouse
        bool timeElapsed = false;
        bool keyPressed = false;

        std::visit(engine::overloaded{
            [&](const engine::CloseWindow &) { window->close(); },
            [&](const engine::TimeElapsed &) { timeElapsed = true; },
            [&](const engine::Pressed<engine::Key> &) { keyPressed = true; },
            [&](const std::monostate &) {},
            [&](const auto &) {}
            },
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

            glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);

        });

    }

    for (const auto &event : eventsProcessed) {
        std::visit(engine::overloaded{
            [](const auto &event_obj) { spdlog::info("Event: {}", event_obj.name); },
            [](const std::monostate &) { spdlog::info("monorail"); } },
            event);
    }

    // otherwise the singleton will be destroy after the main -> dead signal
    holder.instance.reset(nullptr);

    return EXIT_SUCCESS;
}
