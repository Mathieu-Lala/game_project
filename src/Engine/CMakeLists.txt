macro(download_imgui file)
  if(NOT EXISTS "${CMAKE_BINARY_DIR}/download/imgui/${file}")
    file(
      DOWNLOAD
      "https://raw.githubusercontent.com/ocornut/imgui/v1.73/examples/${file}"
      "${CMAKE_BINARY_DIR}/download/imgui/${file}")
  endif()
endmacro()

message(
  STATUS "Downloading from https://raw.githubusercontent.com/ocornut/imgui")
download_imgui(imgui_impl_glfw.cpp)
download_imgui(imgui_impl_glfw.h)
download_imgui(imgui_impl_opengl3.cpp)
download_imgui(imgui_impl_opengl3.h)

add_library(
  engine
  src/Engine/Window.cpp ${CMAKE_BINARY_DIR}/download/imgui/imgui_impl_glfw.cpp
  ${CMAKE_BINARY_DIR}/download/imgui/imgui_impl_opengl3.cpp)
target_include_directories(engine PUBLIC include
                                         ${CMAKE_BINARY_DIR}/download/imgui)
target_link_libraries(
  engine
  PUBLIC project_options
         CONAN_PKG::fmt
         CONAN_PKG::glfw
         CONAN_PKG::glew
         CONAN_PKG::glm
         CONAN_PKG::spdlog
         CONAN_PKG::imgui)

target_compile_definitions(engine PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLEW)
