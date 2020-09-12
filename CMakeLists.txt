cmake_minimum_required(VERSION 3.0[..3.17])

project(proto LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
endif()

set_property(
  CACHE CMAKE_BUILD_TYPE
  PROPERTY STRINGS
           "Debug"
           "Release"
           "MinSizeRel"
           "RelWithDebInfo")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

include(cmake/Conan.cmake)
run_conan()

add_subdirectory(src)
