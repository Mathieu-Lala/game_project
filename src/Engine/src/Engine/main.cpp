#include <spdlog/spdlog.h>

#include <Engine/Event/Event.hpp>        // note : should not require this header here
#include <Engine/Graphics/Shader.hpp>    // note : should not require this header here
#include <Engine/audio/AudioManager.hpp> // note : should not require this header here
#include <Engine/Graphics/Window.hpp>    // note : should not require this header here
#include <Engine/Settings.hpp>           // note : should not require this header here
#include <Engine/component/Color.hpp>
#include <Engine/component/Texture.hpp>
#include <Engine/Core.hpp>

// tmp
#include "ThePURGE.hpp"

int main(int argc, char **argv)
/*try*/ {
    auto holder = engine::Core::Holder::init();

    holder.instance->game<game::ThePURGE>();

    return holder.instance->main(argc, argv);

}/* catch (const std::exception &e) {
    spdlog::error("Caught exception at main level: {}", e.what());
    throw;

} catch (const int code) {
    return code;
}*/
