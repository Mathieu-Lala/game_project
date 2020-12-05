#pragma once

#include <imgui.h>
#include <string_view>

namespace game {

// All fonts can be static because they require no cleanup
struct Fonts {
    static ImFont *imgui;

    static ImFont *kimberley_23;
    static ImFont *kimberley_35;
    static ImFont *kimberley_50;
    static ImFont *kimberley_62;
    static ImFont *opensans_44;
    static ImFont *opensans_32;


    // Must not be called between NewFrame() and EndFrame/Render()
    static void loadFonts() noexcept;

private:
    static ImFont *load(const std::string &simplePath, float size) noexcept;
};

}
