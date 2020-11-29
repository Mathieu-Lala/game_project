#include <spdlog/spdlog.h>
#include <stb_image_write.h>

#include "Engine/Graphics/third_party.hpp"

#include "Engine/Event/Event.hpp"
#include "Engine/Graphics/Shader.hpp"
#include "Engine/Graphics/Window.hpp"
#include "Engine/Event/JoystickManager.hpp"
#include "Engine/audio/AudioManager.hpp" // note : should not require this header here
#include "Engine/Settings.hpp"           // note : should not require this header here
#include "Engine/component/Color.hpp"
#include "Engine/component/VBOTexture.hpp"
#include "Engine/Core.hpp"

engine::Window *engine::Window::s_instance{nullptr};

engine::Window::Window(glm::ivec2 &&size, const std::string_view title, std::uint16_t property) :
    m_monitor{::glfwGetPrimaryMonitor()},
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
    m_handle{::glfwCreateWindow(size.x, size.y, title.data(), nullptr, nullptr)}, m_ui_context{ImGui::CreateContext()}
{
    if (m_handle == nullptr) { throw std::logic_error("Engine::Window initialization failed"); }
    spdlog::trace("Engine::Window instanciated");

    ::glfwGetWindowSize(m_handle, &m_size.x, &m_size.y);
    ::glfwGetWindowPos(m_handle, &m_pos.x, &m_pos.y);

    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = "data/config/imgui.ini";

    setActive();

    if (!ImGui_ImplGlfw_InitForOpenGL(m_handle, false)) {
        throw std::logic_error("Engine::Window initialization glfw failed");
    }

    if (!ImGui_ImplOpenGL3_Init()) { throw std::logic_error("Engine::Window initialization opengl3 failed"); }

    // Vsync
    ::glfwSwapInterval(1);
    ::glEnable(GL_DEPTH_TEST);

    ::glEnable(GL_BLEND);
    ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    s_instance = this;

    ::glfwSetWindowCloseCallback(m_handle, callback_eventClose);
    ::glfwSetWindowSizeCallback(m_handle, callback_eventResized);
    ::glfwSetWindowPosCallback(m_handle, callback_eventMoved);
    ::glfwSetKeyCallback(m_handle, callback_eventKeyBoard);
    ::glfwSetMouseButtonCallback(m_handle, callback_eventMousePressed);
    ::glfwSetCursorPosCallback(m_handle, callback_eventMouseMoved);
    ::glfwSetCharCallback(m_handle, ImGui_ImplGlfw_CharCallback);

    // todo : mouse wheel / request_focus ...

    m_events.emplace_back(OpenWindow{});

    if (property & FULLSCREEN) {
        spdlog::trace("Engine::Window Fullscreen");
        setFullscreen(true);
    }

    ::glViewport(0, 0, m_size.x, m_size.y);
}

engine::Window::~Window()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext(m_ui_context);

    if (m_handle != nullptr) { ::glfwDestroyWindow(m_handle); }

    spdlog::trace("Engine::Window destroyed");
}

auto engine::Window::draw(const std::function<void()> &drawer) -> void
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    drawer();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    render();
}

auto engine::Window::getNextEvent() -> std::optional<Event>
{
    if (m_events.empty()) { return {}; }

    auto nextEvent = m_events.front();
    m_events.erase(m_events.begin());
    return nextEvent;
}

auto engine::Window::isFullscreen() -> bool { return ::glfwGetWindowMonitor(m_handle) != nullptr; }

auto engine::Window::setFullscreen(bool fullscreen) -> void
{
    if (isFullscreen() == fullscreen) return;

    if (fullscreen) {
        ::glfwGetWindowPos(m_handle, &m_pos.x, &m_pos.y);
        ::glfwGetWindowSize(m_handle, &m_size.x, &m_size.y);

        const auto mode = ::glfwGetVideoMode(::glfwGetPrimaryMonitor());

        ::glfwSetWindowMonitor(m_handle, m_monitor, 0, 0, mode->width, mode->height, 0);
    } else {
        ::glfwSetWindowMonitor(m_handle, nullptr, m_pos.x, m_pos.y, m_size.x, m_size.y, GLFW_DONT_CARE);
    }
}

bool engine::Window::screenshot(const std::string_view filename)
{
    GLint viewport[4];

    ::glGetIntegerv(GL_VIEWPORT, viewport);
    const auto &x = viewport[0];
    const auto &y = viewport[1];
    const auto &width = viewport[2];
    const auto &height = viewport[3];

    constexpr auto CHANNEL = 4ul;
    std::vector<char> pixels(static_cast<std::size_t>(width * height) * CHANNEL);

    ::glPixelStorei(GL_PACK_ALIGNMENT, 1);
    ::glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    std::array<char, CHANNEL> pixel;
    for (auto j = 0; j < height / 2; ++j)
        for (auto i = 0; i < width; ++i) {
            const auto top = static_cast<std::size_t>(i + j * width) * pixel.size();
            const auto bottom = static_cast<std::size_t>(i + (height - j - 1) * width) * pixel.size();

            std::memcpy(pixel.data(),           pixels.data() + top,    pixel.size());
            std::memcpy(pixels.data() + top,    pixels.data() + bottom, pixel.size());
            std::memcpy(pixels.data() + bottom, pixel.data(),           pixel.size());
        }

    return !!::stbi_write_png(filename.data(), width, height, 4, pixels.data(), 0);
}

auto engine::Window::isOpen() const -> bool { return ::glfwWindowShouldClose(m_handle) == GLFW_FALSE; }

auto engine::Window::close() -> void { ::glfwSetWindowShouldClose(m_handle, GLFW_TRUE); }

auto engine::Window::render() -> void { ::glfwSwapBuffers(m_handle); }

auto engine::Window::setActive() -> void { ::glfwMakeContextCurrent(m_handle); }

auto engine::Window::setSize(glm::ivec2 &&size) -> void {
    ::glfwSetWindowSize(m_handle, size.x, size.y);
    ::glViewport(0, 0, size.x, size.y);
    m_size = size;
}

auto engine::Window::setPosition(glm::ivec2 &&pos) -> void { ::glfwSetWindowPos(m_handle, pos.x, pos.y); }

auto engine::Window::setCursorPosition(glm::dvec2 &&pos) -> void { ::glfwSetCursorPos(m_handle, pos.x, pos.y); }


auto engine::Window::callback_eventClose([[maybe_unused]] GLFWwindow *window) -> void
{
    IF_RECORD(s_instance->m_events.emplace_back(CloseWindow{}));
}

auto engine::Window::callback_eventResized([[maybe_unused]] GLFWwindow *window, int w, int h) -> void
{
    IF_RECORD(s_instance->m_events.emplace_back(ResizeWindow{w, h}));
}

auto engine::Window::callback_eventMoved([[maybe_unused]] GLFWwindow *window, int x, int y) -> void
{
    IF_RECORD(s_instance->m_events.emplace_back(MoveWindow{x, y}));
}

auto engine::Window::callback_eventKeyBoard([[maybe_unused]] GLFWwindow *window, int key, int scancode, int action, int mods)
    -> void
{
    IF_RECORD(
        // clang-format off
        Key k{
            .alt        = !!(mods & GLFW_MOD_ALT), // NOLINT
            .control    = !!(mods & GLFW_MOD_CONTROL), // NOLINT
            .system     = !!(mods & GLFW_MOD_SUPER), // NOLINT
            .shift      = !!(mods & GLFW_MOD_SHIFT), // NOLINT
            .scancode   = scancode,
            .key        = key
        };
        // clang-format on
        switch (action) {
            case GLFW_PRESS: s_instance->m_events.emplace_back(Pressed<Key>{k}); break;
            case GLFW_RELEASE:
                s_instance->m_events.emplace_back(Released<Key>{k});
                break;
                // case GLFW_REPEAT: s_instance->m_events.emplace_back(???{ key }); break; // todo
                // default: std::abort(); break;
        });
}

auto engine::Window::callback_eventMousePressed(GLFWwindow *window, int button, int action, [[maybe_unused]] int mods) -> void
{
    IF_RECORD(
        // NOLINTNEXTLINE
        double x = 0; double y = 0;
        // NOLINTNEXTLINE
        ::glfwGetCursorPos(window, &x, &y);
        switch (action) {
            case GLFW_PRESS: s_instance->m_events.emplace_back(Pressed<MouseButton>{button, {x, y}}); break;
            case GLFW_RELEASE:
                s_instance->m_events.emplace_back(Released<MouseButton>{button, {x, y}});
                break;
                // default: std::abort(); break;
        });
}

auto engine::Window::callback_eventMouseMoved([[maybe_unused]] GLFWwindow *window, double x, double y) -> void
{
    IF_RECORD(s_instance->m_events.emplace_back(Moved<Mouse>{x, y}));
}

using namespace engine;

template<>
auto Window::applyEvent(const Pressed<MouseButton> &m) -> void
{
    ImGui_ImplGlfw_MouseButtonCallback(m_handle, m.source.button, GLFW_PRESS, 0 /* todo */);
}

template<>
auto Window::applyEvent(const Released<MouseButton> &m) -> void
{
    ImGui_ImplGlfw_MouseButtonCallback(m_handle, m.source.button, GLFW_RELEASE, 0 /* todo */);
}

template<>
auto Window::applyEvent(const Pressed<Key> &k) -> void
{
    ImGui_ImplGlfw_KeyCallback(m_handle, k.source.key, k.source.scancode, GLFW_PRESS, 0 /* todo */);
}

template<>
auto Window::applyEvent(const Released<Key> &k) -> void
{
    ImGui_ImplGlfw_KeyCallback(m_handle, k.source.key, k.source.scancode, GLFW_RELEASE, 0 /* todo */);
}
