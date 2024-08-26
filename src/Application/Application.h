#pragma once

#include "Graphics/Vulkan/VulkanGraphics.h"

class Application
{
public:
	Application();

	~Application();

	void Run();
private:
	WindowHandle m_window;
	VulkanGraphics m_graphics;
};