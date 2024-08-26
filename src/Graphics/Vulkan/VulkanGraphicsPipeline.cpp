#include "VulkanGraphics.h"

void CreateVulkanGraphicsPipeline(VkPipeline& vk_graphicsPipeline, const VkGraphicsPipelineCreateInfo& vk_pipelineInfo,
	const VkDevice& vk_device)
{
	VkResult vk_pipelineCreationResult = vkCreateGraphicsPipelines(vk_device, nullptr, 1, &vk_pipelineInfo, nullptr,
		&vk_graphicsPipeline);
	if (vk_pipelineCreationResult != VK_SUCCESS)
	{
		__debugbreak();
	}
}