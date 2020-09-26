#include <Engine/Core.hpp>

int main(int ac, char **av)
{
    engine::Core::Holder holder;

    holder.instance->window(glm::ivec2{ 400, 400 }, "Hello world #1");

    holder.instance->main(ac, av);

    // otherwise the singleton will be destroy after the main -> dead signal
    holder.instance.reset(nullptr);
}
