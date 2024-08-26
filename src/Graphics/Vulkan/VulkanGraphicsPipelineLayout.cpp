#include "VulkanGraphics.h"

void CreateVulkanGraphicsPipelineLayout(const VkPipelineLayoutCreateInfo& vk_pipelineLayoutInfo, const VkDevice& vk_device,
	VkPipelineLayout& vk_pipelineLayout)
{
	VkResult vk_pipelineLayoutCreationResult = vkCreatePipelineLayout(vk_device, &vk_pipelineLayoutInfo, nullptr, 
		&vk_pipelineLayout);
	if (vk_pipelineLayoutCreationResult != VK_SUCCESS)
	{
		__debugbreak();
	}
}