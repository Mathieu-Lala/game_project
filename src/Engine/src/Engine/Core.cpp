#include "Engine/Core.hpp"

std::unique_ptr<engine::Core> &engine::Core::s_instance = engine::Core::get();
