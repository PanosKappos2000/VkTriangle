#include "VulkanGraphics.h"

void CreateVulkanSurface(const VkInstance& vk_instance, const WindowHandle& window, VkSurfaceKHR& vk_surface)
{
	window.CreateVulkanWindowSurface(vk_instance, vk_surface);
}