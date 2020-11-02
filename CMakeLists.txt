cmake_minimum_required(VERSION 3.13[..3.18])

# Get version from the file 'VERSION'
file(STRINGS "VERSION" PROJECT_VERSION LIMIT_COUNT 1)

project(
  GameProject
  LANGUAGES CXX
  VERSION ${PROJECT_VERSION}
  DESCRIPTION "Game Project - KMU Fall 2020"
  HOMEPAGE_URL "https://github.com/Mathieu-Lala/game_project")

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
add_library(project_pch INTERFACE)

option(ENABLE_PCH "Enable Precompiled Headers" OFF)
if(ENABLE_PCH)
  target_precompile_headers(project_pch INTERFACE <vector> <string> <utility> <functional> <variant> <glm/glm.hpp>)
endif()

# Allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

# External libaries
include(cmake/Conan.cmake)
run_conan()

# Our targets
add_subdirectory(src)

option(ENABLE_TESTING "Enable Test Builds" OFF)
if(ENABLE_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()
