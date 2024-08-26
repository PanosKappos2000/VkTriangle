#include "VulkanGraphics.h"

VulkanSyncObjects::VulkanSyncObjects()
	:vk_imageAvailableSemaphore(), vk_renderFinishedSemaphore(), vk_framesInFlightFence()
{

}

VulkanSyncObjects::~VulkanSyncObjects()
{

}

void VulkanSyncObjects::CreateSyncObjects(const VkDevice& vk_device)
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	vkCreateSemaphore(vk_device, &semaphoreInfo, nullptr, &vk_imageAvailableSemaphore);
	vkCreateSemaphore(vk_device, &semaphoreInfo, nullptr, &vk_renderFinishedSemaphore);

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	//The fence will start as already signaled since we do not want to wait for it on the first draw command
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	vkCreateFence(vk_device, &fenceInfo, nullptr, &vk_framesInFlightFence);
}

void VulkanSyncObjects::Cleanup(const VkDevice& vk_device)
{
	vkDestroySemaphore(vk_device, vk_imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(vk_device, vk_renderFinishedSemaphore, nullptr);

	vkDestroyFence(vk_device, vk_framesInFlightFence, nullptr);
}