set(imgui_version "1.78")

macro(download_imgui file)
  if(NOT EXISTS "${CMAKE_BINARY_DIR}/download/imgui/${imgui_version}/${file}")
    message(
      STATUS
        "Downloading from https://github.com/ocornut/imgui/tree/v${imgui_version}/examples/${file}"
    )
    file(
      DOWNLOAD
      "https://raw.githubusercontent.com/ocornut/imgui/v${imgui_version}/examples/${file}"
      "${CMAKE_BINARY_DIR}/download/imgui/${imgui_version}/${file}")
  endif()
endmacro()

download_imgui(imgui_impl_glfw.cpp)
download_imgui(imgui_impl_glfw.h)
download_imgui(imgui_impl_opengl3.cpp)
download_imgui(imgui_impl_opengl3.h)

add_library(
  glfw_imgui_impl STATIC
  ${CMAKE_BINARY_DIR}/download/imgui/${imgui_version}/imgui_impl_glfw.cpp
  ${CMAKE_BINARY_DIR}/download/imgui/${imgui_version}/imgui_impl_opengl3.cpp)
target_link_libraries(glfw_imgui_impl PUBLIC project_options CONAN_PKG::glew
                                             CONAN_PKG::glfw CONAN_PKG::imgui)
target_compile_definitions(glfw_imgui_impl PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)

configure_file(include/Engine/details/Version.hpp.in
               include/Engine/details/Version.hpp @ONLY)

add_library(
  engine STATIC
  src/Engine/Core.cpp
  src/Engine/Graphics/Window.cpp
  src/Engine/Graphics/Image.cpp
  src/Engine/Graphics/Shader.cpp
  src/Engine/helpers/DrawableFactory.cpp
  src/Engine/Camera.cpp
  src/Engine/Component.cpp
  src/Engine/JoystickManager.cpp 
  src/Engine/audio/AudioManager.cpp 
  src/Engine/audio/AlErrorHandling.cpp
  src/Engine/audio/Sound.cpp
  src/Engine/audio/WavReader.cpp
  src/Engine/Resources.cpp
  src/Engine/audio/AudioFileBuffer.cpp
  src/Engine/DataConfigLoader.cpp
  )

target_include_directories(
  engine PUBLIC include ${CMAKE_BINARY_DIR}/download/imgui/${imgui_version}
                ${CMAKE_CURRENT_BINARY_DIR}/include)
target_link_libraries(
  engine
  PUBLIC project_options
         project_warnings
         project_pch
         glfw_imgui_impl
         CONAN_PKG::glm
         CONAN_PKG::fmt
         CONAN_PKG::spdlog
         CONAN_PKG::nlohmann_json
         CONAN_PKG::entt
         CONAN_PKG::stb
         CONAN_PKG::magic_enum
         CONAN_PKG::CLI11
         CONAN_PKG::openal)

if(MSVC)
  target_compile_definitions(engine PUBLIC NOMINMAX)
endif()
