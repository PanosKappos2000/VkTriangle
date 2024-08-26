#include "VulkanGraphics.h"

void PickPhysicalDevice(const VkInstance& vk_instance, const VkSurfaceKHR& vk_surface, VkPhysicalDevice& vk_GraphicsCard,
	QueueFamilyIndices& gpuQueueFamilyIndices,const std::vector<const char*>& requiredDeviceExtensions, 
	SwapchainSupportDetails& gpuSwapchainSupport)
{
	uint32_t graphicsCardsCount;
	vkEnumeratePhysicalDevices(vk_instance, &graphicsCardsCount, nullptr);
	std::vector<VkPhysicalDevice> graphicsCards(graphicsCardsCount);
	vkEnumeratePhysicalDevices(vk_instance, &graphicsCardsCount, graphicsCards.data());
	for (uint32_t i = 0; i < graphicsCards.size(); ++i)
	{
		if (CheckGraphicsCardQueueFamilies(graphicsCards[i], gpuQueueFamilyIndices, vk_surface) &&
			CheckGraphicsCardExtensionsSupport(graphicsCards[i], requiredDeviceExtensions) && 
			CheckGraphicsCardSwapchainSupport(graphicsCards[i], vk_surface, gpuSwapchainSupport))
		{
			vk_GraphicsCard = graphicsCards[i];
			break;
		}
	}

	if (vk_GraphicsCard == VK_NULL_HANDLE)
	{
		__debugbreak();
	}
}

bool CheckGraphicsCardQueueFamilies(const VkPhysicalDevice& vk_graphicsCard, QueueFamilyIndices& gpuQueueFamilyIndices, 
	const VkSurfaceKHR& vk_surface)
{
	uint32_t queueFamilyPropertiesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(vk_graphicsCard, &queueFamilyPropertiesCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertiesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(vk_graphicsCard, &queueFamilyPropertiesCount, queueFamilyProperties.data());

	//Checking for graphics family index
	bool graphicsFamilyFound = false;
	for (size_t i = 0; i < queueFamilyProperties.size(); ++i)
	{
		if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphicsFamilyFound = true;
			gpuQueueFamilyIndices.graphics = static_cast<uint32_t>(i);
			break;
		}
	}

	//Checking for present family index
	bool presentFamilyFound = false;
	for (uint32_t i = 0; i < queueFamilyProperties.size(); ++i)
	{
		VkBool32 presentationSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(vk_graphicsCard, i, vk_surface,
			&presentationSupport);
		if (presentationSupport)
		{
			presentFamilyFound = true;
			gpuQueueFamilyIndices.present = i;
			break;
		}
	}

	return graphicsFamilyFound && presentFamilyFound;
}

bool CheckGraphicsCardExtensionsSupport(const VkPhysicalDevice& vk_graphicsCard,
	const std::vector<const char*>& requiredDeviceExtensions)
{
	//Getting the available extensions supported by the device
	uint32_t availableExtensionCount = 0;
	vkEnumerateDeviceExtensionProperties(vk_graphicsCard, nullptr, &availableExtensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
	vkEnumerateDeviceExtensionProperties(vk_graphicsCard, nullptr, &availableExtensionCount, availableExtensions.data());

	//Checking that the extensions that our application needs are supported by the device
	std::set<std::string> requiredExtensionsHolder(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end());
	for (const VkExtensionProperties& extension : availableExtensions)
	{
		requiredExtensionsHolder.erase(extension.extensionName);
	}
	return requiredExtensionsHolder.empty();
}

bool CheckGraphicsCardSwapchainSupport(const VkPhysicalDevice& vk_graphicsCard, const VkSurfaceKHR& vk_surface,
	SwapchainSupportDetails& gpuSwapchainSupport)
{
	//Retrieving the surface support from the device
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_graphicsCard, vk_surface, &gpuSwapchainSupport.surfaceCapabilities);

	//Retrieving the surface formats supported by the device
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vk_graphicsCard, vk_surface, &formatCount, nullptr);
	if (formatCount)
	{
		gpuSwapchainSupport.surfaceFormats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(vk_graphicsCard, vk_surface, &formatCount, 
			gpuSwapchainSupport.surfaceFormats.data());
	}

	//Retrieving the available present modes supported by the device
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(vk_graphicsCard, vk_surface, &presentModeCount, nullptr);
	if (presentModeCount)
	{
		gpuSwapchainSupport.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(vk_graphicsCard, vk_surface, &presentModeCount,
			gpuSwapchainSupport.presentModes.data());
	}

	return !(gpuSwapchainSupport.presentModes.empty()) && !(gpuSwapchainSupport.surfaceFormats.empty());
}

void CreateVulkanLogicalDevice(VkDevice& vk_device, const VkDeviceCreateInfo& vk_deviceInfo,
	const VkPhysicalDevice& vk_graphicsCard)
{
	VkResult deviceCreationResult = vkCreateDevice(vk_graphicsCard, &vk_deviceInfo, nullptr, &vk_device);
	if (deviceCreationResult != VK_SUCCESS)
	{
		__debugbreak();
	}
}
