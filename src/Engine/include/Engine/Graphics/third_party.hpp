#pragma once

// just an helper file to have the header in the right order

#define GLFW_INCLUDE_NONE
#ifdef _WIN32
# define GLFW_EXPOSE_NATIVE_WIN32
# include <windows.h>
#endif

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
