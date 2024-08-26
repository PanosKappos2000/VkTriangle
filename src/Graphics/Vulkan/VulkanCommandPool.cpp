#include "VulkanGraphics.h"

void CreateVulkanCommandPool(VkCommandPool& vk_commandPool, const VkCommandPoolCreateInfo& vk_commandPoolInfo,
	const VkDevice& vk_device)
{
	VkResult vk_commandPoolCreationResult = vkCreateCommandPool(vk_device, &vk_commandPoolInfo, nullptr, &vk_commandPool);
	if (vk_commandPoolCreationResult != VK_SUCCESS)
	{
		__debugbreak();
	}
}