#include "VulkanGraphics.h"

void CreateVulkanSwapchainImageViews(VkImageView& vk_imageView, const VkImageViewCreateInfo& vk_imageViewInfo,
	const VkDevice& vk_device)
{
	VkResult vk_imageViewCreationResult = vkCreateImageView(vk_device, &vk_imageViewInfo, nullptr, &vk_imageView);
	if (vk_imageViewCreationResult != VK_SUCCESS)
	{
		__debugbreak();
	}
}