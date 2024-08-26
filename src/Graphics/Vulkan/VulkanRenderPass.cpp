#include "VulkanGraphics.h"

void CreateVulkanRenderPass(VkRenderPass& vk_renderPass, const VkRenderPassCreateInfo& vk_renderPassInfo,
	const VkDevice& vk_device)
{
	VkResult vk_renderPassCreationResult = vkCreateRenderPass(vk_device, &vk_renderPassInfo, nullptr, &vk_renderPass);
	if (vk_renderPassCreationResult != VK_SUCCESS)
	{
		__debugbreak();
	}
}

void CreateVulkanRenderPassBeginInfo(VkRenderPassBeginInfo& vk_renderPassBegin, const VkFramebuffer& vk_framebuffer,
	const VkRenderPass& vk_renderPass, const VkExtent2D& vk_imageExtent, const VkOffset2D& vk_rect,
	uint32_t clearValueCount, VkClearValue* vk_clearValue)
{
	vk_renderPassBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	vk_renderPassBegin.framebuffer = vk_framebuffer;
	vk_renderPassBegin.renderPass = vk_renderPass;
	vk_renderPassBegin.renderArea.offset = vk_rect;
	vk_renderPassBegin.renderArea.extent = vk_imageExtent;
	vk_renderPassBegin.clearValueCount = clearValueCount;
	vk_renderPassBegin.pClearValues = vk_clearValue;
}