#pragma once

#include "engine_core_export.h"

namespace engine {

class Core;

namespace api {

ENGINE_CORE_EXPORT auto getCore() -> engine::Core *;

} // namespace api

} // namespace engine
