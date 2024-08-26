#include "Window.h"

WindowHandle::WindowHandle()
	:glfw_window{nullptr}, m_width{0}, m_height{0}
{

}

WindowHandle::~WindowHandle()
{
	
}

void WindowHandle::Init()
{
	int glfwInitResult = glfwInit();
	if (!glfwInitResult)
	{
		__debugbreak();
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_width = 720;
	m_height = 560;
	glfw_window = glfwCreateWindow(m_width, m_height, "Vulkan", nullptr, nullptr);

	glfwSetWindowUserPointer(glfw_window, this);
}

void WindowHandle::CreateVulkanWindowSurface(const VkInstance& vk_instance, VkSurfaceKHR& vk_surface) const
{
	VkResult surfaceCreationResult = glfwCreateWindowSurface(vk_instance, glfw_window, nullptr, &vk_surface);
	if (surfaceCreationResult != VK_SUCCESS)
	{
		__debugbreak();
	}
}

void WindowHandle::MainLoop()
{
	glfwPollEvents();
}

void WindowHandle::Cleanup()
{
	glfwDestroyWindow(glfw_window);
	glfwTerminate();
}