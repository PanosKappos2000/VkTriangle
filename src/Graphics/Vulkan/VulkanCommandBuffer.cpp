#include "VulkanGraphics.h"


void AllocateVulkanCommandBuffer(VkCommandBuffer& vk_commandBuffer,const VkDevice& vk_device, 
	const VkCommandBufferAllocateInfo& vk_commandBufferInfo)
{
	VkResult vk_commandBufferAllocResult = vkAllocateCommandBuffers(vk_device, &vk_commandBufferInfo, 
		&vk_commandBuffer);
	if (vk_commandBufferAllocResult != VK_SUCCESS)
	{
		__debugbreak();
	}
}

void CreateVulkanCommandBufferBeginInfo(VkCommandBufferBeginInfo& vk_commandBufferBegin,
	VkCommandBufferUsageFlags vk_commandBufferUsage, VkCommandBufferInheritanceInfo* vk_commandBufferInheritance)
{
	vk_commandBufferBegin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vk_commandBufferBegin.flags = vk_commandBufferUsage;
	vk_commandBufferBegin.pInheritanceInfo = vk_commandBufferInheritance;
}

void RecordCommandBuffer(const VkCommandBufferBeginInfo& vk_commandBufferBegin, const VkRenderPassBeginInfo& vk_renderPassBegin,
	const VkCommandBuffer& vk_commandBuffer, const VkPipeline& vk_graphicsPipeline,
	const VkExtent2D vk_imageExtent)
{
	vkBeginCommandBuffer(vk_commandBuffer, &vk_commandBufferBegin);
	vkCmdBeginRenderPass(vk_commandBuffer, &vk_renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(vk_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_graphicsPipeline);

	//Setting the dynamic state of the pipeline that we specified during its creation
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(vk_imageExtent.width);
	viewport.height = static_cast<float>(vk_imageExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(vk_commandBuffer, 0, 1, &viewport);
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = vk_imageExtent;
	vkCmdSetScissor(vk_commandBuffer, 0, 1, &scissor);

	vkCmdDraw(vk_commandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(vk_commandBuffer);
	vkEndCommandBuffer(vk_commandBuffer);
}