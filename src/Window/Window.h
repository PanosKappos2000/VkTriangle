#pragma once

#include "Graphics/Vulkan/glfwVulkan.h"

class WindowHandle
{
public:
	WindowHandle();

	~WindowHandle();
	
	/* Initializes glfw, creates the window and adds other functionality like function callbacks for different events and 
	* loads the user pointer to memory */
	void Init();

	// Called when initializing vulkan to create a window surface to interface with the window
	void CreateVulkanWindowSurface(const VkInstance& vk_instance, VkSurfaceKHR& vk_surface) const;

	void Cleanup();

	//Getter functions....
	inline int GetWidth() const { return m_width; }

	inline int GetHeight() const { return m_height; }

	inline const GLFWwindow* GetGLFWwindow() { return glfw_window; }
	//....End getter functions

	//Calls any functions needed to update the window system
	void MainLoop();

	inline bool ShouldClose() const { return glfwWindowShouldClose(glfw_window); }

private:

private:
	GLFWwindow* glfw_window;

	int m_width;
	int m_height;
};