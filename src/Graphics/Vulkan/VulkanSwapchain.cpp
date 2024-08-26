#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp
#include "VulkanGraphics.h"

void CreateVulkanSwapchain(VkSwapchainKHR& vk_swapchain, const VkDevice& vk_device,
    const VkSwapchainCreateInfoKHR& vk_swapchainInfo)
{
    VkResult vk_swapchainCreationResult = vkCreateSwapchainKHR(vk_device, &vk_swapchainInfo, nullptr, &vk_swapchain);
    if (vk_swapchainCreationResult != VK_SUCCESS)
    {
        __debugbreak();
    }
}

void ChooseVulkanSurfaceFormat(VkSurfaceFormatKHR& vk_surfaceFormat,
    const std::vector< VkSurfaceFormatKHR>& availableSurfaceFormats)
{
    for (uint32_t i = 0; i < availableSurfaceFormats.size(); ++i)
    {
        if (availableSurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableSurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            vk_surfaceFormat = availableSurfaceFormats[i];
        }
    }

    vk_surfaceFormat = availableSurfaceFormats[0];
}

void ChooseVulkanSwapchainPresentMode(VkPresentModeKHR& vk_swapchainPresentMode,
    const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (uint32_t i = 0; i < availablePresentModes.size(); ++i)
    {
        if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            vk_swapchainPresentMode = availablePresentModes[i];
            return;
        }
    }

    vk_swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
}

void ChooseVulkanSwapchainExtent(VkExtent2D& vk_swapchainExtent, const VkSurfaceCapabilitiesKHR& vk_surfaceCapabilities,
    uint32_t windowWidth, uint32_t windowHeight)
{
    if (vk_surfaceCapabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
    {
        vk_swapchainExtent = vk_surfaceCapabilities.currentExtent;
    }
    else
    {
        VkExtent2D actualExtent =
        {
            windowWidth, windowHeight
        };

        actualExtent.width = std::clamp(actualExtent.width,
            vk_surfaceCapabilities.minImageExtent.width, vk_surfaceCapabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height,
            vk_surfaceCapabilities.minImageExtent.height, vk_surfaceCapabilities.maxImageExtent.height);

        vk_swapchainExtent = actualExtent;
    }
}