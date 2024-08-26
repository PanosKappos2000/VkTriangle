#include "VulkanGraphics.h"

void CreateVulkanSubmitInfo(VkSubmitInfo& vk_submitInfo, uint32_t waitSemaphoreCount, VkSemaphore* vk_waitSemaphores,
	VkPipelineStageFlags* vk_waitStages, uint32_t commandBufferCount, const VkCommandBuffer& vk_commandBuffer,
	uint32_t signalSemaphoreCount, VkSemaphore* vk_signalSemaphores)
{
	vk_submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	vk_submitInfo.waitSemaphoreCount = waitSemaphoreCount;
	vk_submitInfo.pWaitSemaphores = vk_waitSemaphores;
	vk_submitInfo.commandBufferCount = commandBufferCount;
	vk_submitInfo.pCommandBuffers = &vk_commandBuffer;
	vk_submitInfo.pWaitDstStageMask = vk_waitStages;
	vk_submitInfo.signalSemaphoreCount = signalSemaphoreCount;
	vk_submitInfo.pSignalSemaphores = vk_signalSemaphores;
}

void CreateVulkanPresentInfo(VkPresentInfoKHR& vk_presentInfo, uint32_t waitSemaphoreCount, VkSemaphore* vk_waitSemaphores,
	uint32_t swapchainCount, VkSwapchainKHR* vk_swapchains, uint32_t imageIndex)
{
	vk_presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	vk_presentInfo.waitSemaphoreCount = waitSemaphoreCount;
	vk_presentInfo.pWaitSemaphores = vk_waitSemaphores;
	vk_presentInfo.swapchainCount = swapchainCount;
	vk_presentInfo.pSwapchains = vk_swapchains;
	vk_presentInfo.pImageIndices = &imageIndex;
	vk_presentInfo.pResults = nullptr;
}