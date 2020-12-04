#include <Engine/Core.hpp>

#include "widgets/Fonts.hpp"

ImFont *game::Fonts::kimberley_23;
ImFont *game::Fonts::kimberley_35;
ImFont *game::Fonts::kimberley_50;
ImFont *game::Fonts::kimberley_62;
ImFont *game::Fonts::opensans_44;
ImFont *game::Fonts::opensans_32;
ImFont *game::Fonts::imgui;

void game::Fonts::loadFonts() noexcept
{
    imgui = ImGui::GetIO().Fonts->AddFontDefault();

    kimberley_23 = load("fonts/kimberley_bl.ttf", 23.f);
    kimberley_35 = load("fonts/kimberley_bl.ttf", 35.f);
    kimberley_50 = load("fonts/kimberley_bl.ttf", 50.f);
    kimberley_62 = load("fonts/kimberley_bl.ttf", 62.f);

    opensans_44 = load("fonts/OpenSans.ttf", 44.f);
    opensans_32 = load("fonts/OpenSans.ttf", 32.f);
}

ImFont *game::Fonts::load(const std::string &simplePath, float size) noexcept
{
    static auto holder = engine::Core::Holder{};

    ImGuiIO &io = ImGui::GetIO();

    return io.Fonts->AddFontFromFileTTF((holder.instance->settings().data_folder + simplePath).c_str(), size);
}
