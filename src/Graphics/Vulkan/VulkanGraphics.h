#pragma once

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <fstream>
#include "Window/Window.h"


/* Functions that initialize and utilize the vulkan SDK instance objects. The instance object (VkInstance) is required 
   in order to interface with the vulkan SDK and for most cases only one instance will exist */
void CreateVulkanInstance(VkInstance* vk_instance, VkInstanceCreateInfo& vk_instanceInfo);

/* Function that calls on the window handle object to call the window specification's own function 
   to initialize the vulkan SDK surface object, need to interface with the window system  */
void CreateVulkanSurface(const VkInstance& vk_instance, const WindowHandle& window, VkSurfaceKHR& vk_surface);




/* Helper struct used to hold the indices of different queue families inside the GPUs that support
   different kinds of commands, like graphics commands and presentation commands */
struct QueueFamilyIndices
{
	uint32_t graphics;
	uint32_t present;
};

/* Helper struct that holds the swap chain support capabilities of a graphics card
   - It holds the min/max amount of swapchain images and mix/max width and height of images that the surface allows
   - It holds the surface format, meaning the pixel formats and the color spaces that the surface supports
   - And it also holds the available presentation modes   */
struct SwapchainSupportDetails
{
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<VkSurfaceFormatKHR> surfaceFormats;
	std::vector<VkPresentModeKHR> presentModes;
};

/* Function called to pick a physical device for vulkan to interface with (required to initialize the VkDevice object that
   will be referenced throughout the application). The function some of the details and features of the device that are of interest
   to the application */
void PickPhysicalDevice(const VkInstance& vk_instance, const VkSurfaceKHR& vk_surface, VkPhysicalDevice& vk_GraphicsCard,
	QueueFamilyIndices& gpuQueueFamilyIndices, const std::vector<const char*>& requiredDeviceExtensions,
	SwapchainSupportDetails& gpuSwapchainSupport);

/*Checks if the graphics card supports the queue families for the commands we need for our application and saves the indices 
  in the queue family indices argument that was passed.*/
bool CheckGraphicsCardQueueFamilies(const VkPhysicalDevice& vk_graphicsCard, QueueFamilyIndices& gpuQueueFamilyIndices, 
	const VkSurfaceKHR& vk_surface);

//Checks if the graphics card support the extensions we need for our application 
bool CheckGraphicsCardExtensionsSupport(const VkPhysicalDevice& vk_graphicsCard,
	const std::vector<const char*>& requiredDeviceExtensions);

/* Checks if the swapchain support that a graphics card offers is adequate for our application and then saves the details
   on the SwapchainSupportDetails struct passed in as argument 3 */
bool CheckGraphicsCardSwapchainSupport(const VkPhysicalDevice& vk_graphicsCard, const VkSurfaceKHR& vk_surface,
	SwapchainSupportDetails& gpuSwapchainSupport);


//Creates a logical device for Vulkan to interface with the actual graphics card
void CreateVulkanLogicalDevice(VkDevice& vk_device, const VkDeviceCreateInfo& vk_deviceInfo,
	const VkPhysicalDevice& vk_graphicsCard);


/* Creates the vulkan swapchain which will own the framebuffers that the application will render to. 
   It is necessary as it holds the queue of images that are waiting to be presented on the screen*/
void CreateVulkanSwapchain(VkSwapchainKHR& vk_swapchain, const VkDevice& vk_device,
	 const VkSwapchainCreateInfoKHR& vk_swapchainInfo);

/*Checks the available surface formats we retrieved from the graphics card and chooses the most suitable one for the application.
  The surface format specifies the pixel format and color space*/
void ChooseVulkanSurfaceFormat(VkSurfaceFormatKHR& vk_surfaceFormat,
	const std::vector< VkSurfaceFormatKHR>& availableSurfaceFormats);

/*Checks the available present modes we retrieved from the graphics card and chooses the most suitable one for the application.
  The present modes are the conditions for swapping images to the screen */
void ChooseVulkanSwapchainPresentMode(VkPresentModeKHR& vk_swapchainPresentMode,
	const std::vector<VkPresentModeKHR>& availablePresentModes);

//Sets the swapchain extent according to the surface capabilities we retrieved for the graphics card
void ChooseVulkanSwapchainExtent(VkExtent2D& vk_swapchainExtent, const VkSurfaceCapabilitiesKHR& vk_surfaceCapabilities,
	uint32_t windowWidth, uint32_t windowHeight);


/* Creates an image view which is need to use an image in the graphics pipeline. An image view object is created for a specific
   image object which will then. The image view describes how to access the image and which part of the image to access */
void CreateVulkanSwapchainImageViews(VkImageView& vk_imageView, const VkImageViewCreateInfo& vk_imageViewInfo,
	const VkDevice& vk_device);

/* Creates an pipeline layout which will be passed into a graphics pipeline object through info struct. The pipeline layout 
   will allow the application to pass uniform variables into a shader */
void CreateVulkanGraphicsPipelineLayout(const VkPipelineLayoutCreateInfo& vk_pipelineLayoutInfo, const VkDevice& vk_device,
	VkPipelineLayout& vk_pipelineLayout);

/* Creates a vulkan render pass object which will specify what kind of attachments a framebuffer will have, how many 
   samples to use for each of them and how their contents will be handled throughout rendering */
void CreateVulkanRenderPass(VkRenderPass& vk_renderPass, const VkRenderPassCreateInfo& vk_renderPassInfo,
	const VkDevice& vk_device);

/* Create a vulkan graphics pipeline object which is one of the pipelines that the application will be utilizing.
   The graphics pipeline is a series of operations that take vertices of the objects that are to be rendered all
   the way to pixels that are presented on screen*/
void CreateVulkanGraphicsPipeline(VkPipeline& vk_graphicsPipeline, const VkGraphicsPipelineCreateInfo& vk_pipelineInfo,
	const VkDevice& vk_device);

/* Creates a vulkan framebuffer object which references the image views that represent the attachments specified 
   in the render pass that was the application will use*/
void CreateVulkanFramebuffer(VkFramebuffer& vk_framebuffer, const VkFramebufferCreateInfo& vk_framebufferInfo,
	const VkDevice& vk_device);

/* Creates a command pool which is necessary for allocating a command buffer which will be used to record the commands
   we want vulkan to execute for our application */
void CreateVulkanCommandPool(VkCommandPool& vk_commandPool, const VkCommandPoolCreateInfo& vk_commandPoolInfo,
	const VkDevice& vk_device);

/* Allocates a command buffer that will be used to record a set of commands so that when the application needs
   vulkan to do something, the commands have already been allocated in a buffer and recorded*/
void AllocateVulkanCommandBuffer(VkCommandBuffer& vk_commandBuffer,const VkDevice& vk_device, 
	const VkCommandBufferAllocateInfo& vk_commandBufferInfo);

void RecordCommandBuffer(const VkCommandBufferBeginInfo& vk_commandBufferBegin, const VkRenderPassBeginInfo& vk_renderPassBegin, 
	const VkCommandBuffer& vk_commandBuffer, const VkPipeline& vk_graphicsPipeline,
	const VkExtent2D vk_imageExtent);


void CreateVulkanCommandBufferBeginInfo(VkCommandBufferBeginInfo& vk_commandBufferBegin,
	VkCommandBufferUsageFlags vk_commandBufferUsage, VkCommandBufferInheritanceInfo* vk_commandBufferInheritance);

void CreateVulkanRenderPassBeginInfo(VkRenderPassBeginInfo& vk_renderPassBegin, const VkFramebuffer& vk_framebuffer,
	const VkRenderPass& vk_renderPass, const VkExtent2D& vk_imageExtent, const VkOffset2D& vk_rect, 
	uint32_t clearValueCount, VkClearValue* vk_clearValue);

void CreateVulkanSubmitInfo(VkSubmitInfo& vk_submitInfo, uint32_t waitSemaphoreCount, VkSemaphore* vk_waitSemaphores,
	VkPipelineStageFlags* vk_waitStages, uint32_t commandBufferCount, const VkCommandBuffer& vk_commandBuffer,
	uint32_t signalSemaphoreCount, VkSemaphore* vk_signalSemaphores);

void CreateVulkanPresentInfo(VkPresentInfoKHR& vk_presentInfo, uint32_t waitSemaphoreCount, VkSemaphore* vk_waitSemaphores,
	uint32_t swapchainCount, VkSwapchainKHR* vk_swapchains, uint32_t imageIndex);



class VulkanSyncObjects
{
public:
	VulkanSyncObjects();
	~VulkanSyncObjects();

	void CreateSyncObjects(const VkDevice& vk_device);
	
	void Cleanup(const VkDevice& vk_device);
public:
	VkSemaphore vk_imageAvailableSemaphore;
	VkSemaphore vk_renderFinishedSemaphore;
	VkFence vk_framesInFlightFence;
};



struct GraphicsPipelineFixedState
{ 
	VkPipelineInputAssemblyStateCreateInfo vk_inputAssemblyInfo;
	VkPipelineViewportStateCreateInfo vk_viewportInfo;
	VkPipelineRasterizationStateCreateInfo vk_rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo vk_multisamplingInfo;
	VkPipelineColorBlendStateCreateInfo vk_colorBlendInfo;
};


class VulkanGraphics
{
public:
	VulkanGraphics();

	~VulkanGraphics();

	/* Initializes all the necessary vulkan object wrappers and sets them so that the main loop of the class
	can run properly and allow the other main loops to function as well   */
	void Init(const WindowHandle& window);

	void Cleanup();

	/* Called repeatedly by the application to for standard graphics operations until certain conditions are met which 
	stop the application   */
	void MainLoop();
private:
	//Called in the main loop to draw graphics
	void Draw();
	
	//Creates a default VkInstanceCreateInfo that is used to create the vulkan instance when the application starts
	void CreateAppDefaultVkInstanceInfo(VkInstanceCreateInfo& vk_instanceInfo, VkApplicationInfo& vk_appInfo);

	//Creates a default VkDeviceCreateInfo that is used to create the logical device when the application starts
	void CreateAppDefaultVkDeviceInfo(VkDeviceCreateInfo& vk_deviceInfo, const QueueFamilyIndices& gpuQueueFamilyIndices,
		const std::vector<const char*>& requiredDeviceExtensions, std::vector<VkDeviceQueueCreateInfo>& queueCreateInfo);

	//Creates a default VkSwapchainInfo that is used to create the swapchain when the application starts
	void CreateAppDefaultVkSwapchainInfo(VkSwapchainCreateInfoKHR& vk_swapchainInfo, const VkSurfaceFormatKHR& vk_surfaceFormat,
		const VkExtent2D& vk_swapchainExtent, const VkPresentModeKHR& vk_presentMode);

	/* Default info functions needed to create the objects that need to be passed into the graphics pipeline in order to 
	   properly create one */
	//Creates the default VkPipelineLayoutCreateInfo that is used to create the pipeline layout when the application starts
	void CreateAppDefaultPipelineLayoutInfo(VkPipelineLayoutCreateInfo& vk_pipelineLayoutInfo);

	//Creates the default VkRenderPassCreateInfo that is used to create the render pass when the application starts
	void CreateAppDefaultRenderPassInfo(VkRenderPassCreateInfo& vk_renderPassInfo, VkAttachmentDescription& vk_attachmentInfo,
		VkAttachmentReference& vk_attachmentRef, VkSubpassDescription& vk_subpassInfo, 
		VkSubpassDependency& vk_subpassDependency);

	void CreateAppDefaultVkPipelineInfo(VkGraphicsPipelineCreateInfo& vk_pipelineInfo,
		const VkPipelineShaderStageCreateInfo* shaderStageInfos,const VkPipelineVertexInputStateCreateInfo& vk_vertexInputStateInfo, 
		const GraphicsPipelineFixedState& fixedState, const VkPipelineDynamicStateCreateInfo& vk_dynamicStateInfo);

	//Creates a default create info for all the fixed state objects of the pipeline.These infos will al be passed to the pipeline
	void CreateAppDefaultPipelineFixedState(VkPipelineInputAssemblyStateCreateInfo& vk_inputAssemblyInfo,
		VkPipelineViewportStateCreateInfo& vk_viewportInfo, VkPipelineRasterizationStateCreateInfo& vk_rasterizationInfo,
		VkPipelineMultisampleStateCreateInfo& vk_multisamplingInfo, VkPipelineColorBlendAttachmentState& vk_colorBlendAttachment,
		VkPipelineColorBlendStateCreateInfo& vk_colorBlendInfo, GraphicsPipelineFixedState& fixedState);

	//Takes the filename of a file and reads the byte code into the array passed in as the 1st argument
	void ReadShaderFile(std::vector<char>& shaderCode, const char* shaderFilename);

	//Creates a shader stage for each of the shaders(the vertex and the fragment). I needs to be passed to the pipeline
	void CreateShaderStages(VkShaderModule& vk_vertexShaderModule, VkShaderModule& vk_fragShaderModule,
		VkPipelineShaderStageCreateInfo& vk_vertexShaderStageInfo, VkPipelineShaderStageCreateInfo& vk_fragShaderStageInfo,
		std::vector<char>& vertexShaderCode, std::vector<char>& fragShaderCode, const VkDevice& vk_device);

	//Creates a default vertex input info that specifies the format of the vertex data that is passed
	void CreateAppDefaultVkVertexInputInfo(VkPipelineVertexInputStateCreateInfo& vk_vertexInputInfo);

	//Creates a default framebuffer info used to create the default framebuffers of the application
	void CreateAppDefaultFramebufferInfo(VkFramebufferCreateInfo& vk_framebufferInfo, uint32_t imageViewIndex);

	//Creates a default command pool info used to create the command pool which will allocate the command buffers
	void CreateAppDefaultVkCommandPoolInfo(VkCommandPoolCreateInfo& vk_commandPoolInfo, uint32_t graphicsQueueFamilyIndex);

	void CreateAppDefaultVkCommandBufferInfo(VkCommandBufferAllocateInfo& vk_commandBufferInfo,
		const VkCommandPool& vk_commandPool);

private:
	//The vulkan instance that the app is going to use to interface with the vulkan SDK
	VkInstance vk_instance;

	//The surface that vulkan is going to use to interface with the window system
	VkSurfaceKHR vk_surface;

	//The graphics card that vulkan is going to interface with and details about it
	VkPhysicalDevice vk_graphicsCard;
	QueueFamilyIndices m_gpuQueueFamilies;
	SwapchainSupportDetails m_gpuSwapchainSupport;
	std::vector<const char*> requiredDeviceExtensions;

	//The vulkan device objects and its queues
	VkDevice vk_device;
	VkQueue vk_graphicsQueue;
	VkQueue vk_presentQueue;

	//The vulkan swapchain object which will own the framebuffers that the app will render to
	VkSwapchainKHR vk_swapchain;
	std::vector<VkImage> swapchainImages;
	VkFormat vk_imageFormat;
	VkExtent2D vk_imageExtent;

	/*The image views created by the application will allow it to view the details of 
	  the image objects retrieved from the swapchain */
	std::vector<VkImageView> imageViews;

	//The pipeline layout allows the application side of the program to pass uniform variables to the shaders 
	VkPipelineLayout vk_pipelineLayout;

	//The render pass specifies the framebuffer attachments that will be used during rendering operations
	VkRenderPass vk_renderPass;

	//Holds the graphics pipeline which will draw our triangle
	VkPipeline vk_graphicsPipeline;

	//Holds the framebuffer which wrap around the attachments specified in the render pass
	std::vector<VkFramebuffer>framebuffers;

	//Holds the command pool which can allocate the command buffers used to execute vulkan commands
	VkCommandPool vk_commandPool;

	//Holds a coomand buffer which records all the vulkan commands which our application needs
	VkCommandBuffer vk_commandBuffer;

	VulkanSyncObjects m_syncObjects;
};

