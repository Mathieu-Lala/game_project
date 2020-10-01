cmake_minimum_required(VERSION 3.9[..3.18])

# Get version from the file 'VERSION'
file(STRINGS "VERSION" PROJECT_VERSION LIMIT_COUNT 1)

project(proto
    LANGUAGES CXX
    VERSION ${PROJECT_VERSION}
    DESCRIPTION "KMU Fall Game Project"
    HOMEPAGE_URL "https://github.com/Pierre-Genest/game_project")

include(cmake/PreventInSourceBuilds.cmake)
include(cmake/StandardProjectSettings.cmake)

# Cache Compiler
include(cmake/Cache.cmake)


# Options to apply on targets
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

# Warnings to apply on targets
add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)


# Debugging flags
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# Compile time optimizer
option(ENABLE_PCH "Enable Precompiled Headers" OFF)
if(ENABLE_PCH)
  target_precompile_headers(project_options INTERFACE
    <vector> <string> <utility> <GL/glew.h> <GLFW/glfw3.h> <functional>)
endif()


# Allow for static analysis options
include(cmake/StaticAnalyzers.cmake)


# External libaries
include(cmake/Conan.cmake)
run_conan()


# Our targets
add_subdirectory(src)
