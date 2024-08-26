#include "VulkanGraphics.h"
#include <vector>
#include <iostream>



void CreateVulkanInstance(VkInstance* vk_instance, VkInstanceCreateInfo& vk_instanceInfo)
{
	//Retrieving and printing available extensions
	uint32_t instanceExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions;
	availableExtensions.resize(instanceExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, availableExtensions.data());
	for (const VkExtensionProperties& extension : availableExtensions)
	{
		std::cout << extension.extensionName << '\n';
	}

	//Enabling required extensions
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	vk_instanceInfo.enabledExtensionCount = glfwExtensionCount;
	vk_instanceInfo.ppEnabledExtensionNames = glfwExtensions;
	
	VkResult vk_instanceCreationResult = vkCreateInstance(&vk_instanceInfo, nullptr, vk_instance);
	if (vk_instanceCreationResult != VK_SUCCESS)
	{
		__debugbreak();
	}
}