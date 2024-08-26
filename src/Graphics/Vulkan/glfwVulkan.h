#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
//GLFW will include its own definitions and automatically load the Vulkan header with it
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>