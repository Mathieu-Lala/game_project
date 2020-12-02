#include <Engine/Core.hpp>

#include "widgets/Fonts.hpp"

ImFont *game::Fonts::kimberley_23;
ImFont *game::Fonts::imguiDefault;

void game::Fonts::loadFonts() noexcept
{
    imguiDefault = ImGui::GetIO().Fonts->AddFontDefault();

    kimberley_23 = load("fonts/kimberley_bl.ttf", 23.f);
}

ImFont *game::Fonts::load(const std::string &simplePath, float size) noexcept
{
    static auto holder = engine::Core::Holder{};

    ImGuiIO &io = ImGui::GetIO();

    return io.Fonts->AddFontFromFileTTF((holder.instance->settings().data_folder + simplePath).c_str(), size);
}
