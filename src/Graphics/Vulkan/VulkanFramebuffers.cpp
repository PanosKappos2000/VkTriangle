#include "VulkanGraphics.h"


void CreateVulkanFramebuffer(VkFramebuffer& vk_framebuffer, const VkFramebufferCreateInfo& vk_framebufferInfo,
	const VkDevice& vk_device)
{
	VkResult vk_framebufferCreationResult = vkCreateFramebuffer(vk_device, &vk_framebufferInfo, nullptr, &vk_framebuffer);
	if (vk_framebufferCreationResult != VK_SUCCESS)
	{
		__debugbreak();
	}
}