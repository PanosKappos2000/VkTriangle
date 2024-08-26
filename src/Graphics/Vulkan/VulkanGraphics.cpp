#include "VulkanGraphics.h"


VulkanGraphics::VulkanGraphics()
	:vk_instance(), vk_surface(), vk_graphicsCard(VK_NULL_HANDLE), m_gpuQueueFamilies(), m_gpuSwapchainSupport(),
	requiredDeviceExtensions(), vk_device(), vk_graphicsQueue(), vk_presentQueue(), vk_swapchain(), swapchainImages(),
	vk_imageFormat(), vk_imageExtent(), imageViews(), vk_pipelineLayout(), vk_renderPass(),vk_graphicsPipeline(),
	framebuffers(), vk_commandPool(), vk_commandBuffer(), m_syncObjects()
{
	
}

VulkanGraphics::~VulkanGraphics()
{
	vkDestroyCommandPool(vk_device, vk_commandPool, nullptr);
	for (uint32_t i = 0; i < framebuffers.size(); ++i)
	{
		vkDestroyFramebuffer(vk_device, framebuffers[i], nullptr);
	}
	vkDestroyPipeline(vk_device, vk_graphicsPipeline, nullptr);
	vkDestroyRenderPass(vk_device, vk_renderPass, nullptr);
	vkDestroyPipelineLayout(vk_device, vk_pipelineLayout, nullptr);
	for (uint32_t i = 0; i < imageViews.size(); ++i)
	{
		vkDestroyImageView(vk_device, imageViews[i], nullptr);
	}
	vkDestroySwapchainKHR(vk_device, vk_swapchain, nullptr);
	vkDestroyDevice(vk_device, nullptr);
	vkDestroySurfaceKHR(vk_instance, vk_surface, nullptr);
	vkDestroyInstance(vk_instance, nullptr);
}

void VulkanGraphics::Init(const WindowHandle& window)
{
	//Initializing an instance first so that the application can interface with the vulkan API
	VkInstanceCreateInfo vk_instanceInfo{};
	VkApplicationInfo vk_appInfo{};
	CreateAppDefaultVkInstanceInfo(vk_instanceInfo, vk_appInfo);
	CreateVulkanInstance(&vk_instance, vk_instanceInfo);

	//Creating the surface so that vulkan can interface with the window system
	CreateVulkanSurface(vk_instance, window, vk_surface);

	//Picking a physical device/graphics card for Vulkan to interface with
	requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	PickPhysicalDevice(vk_instance, vk_surface, vk_graphicsCard, m_gpuQueueFamilies, 
		requiredDeviceExtensions, m_gpuSwapchainSupport);

	//Creating the VkDevice(logical device) object that will interface with the physical device we picked earlier
	VkDeviceCreateInfo vk_deviceInfo{};
	std::vector<VkDeviceQueueCreateInfo> queueInfos;
	CreateAppDefaultVkDeviceInfo(vk_deviceInfo, m_gpuQueueFamilies, requiredDeviceExtensions, queueInfos);
	CreateVulkanLogicalDevice(vk_device, vk_deviceInfo, vk_graphicsCard);
	//Retrieving the queue from the device object based on the queue family indices we got from the physical device
	vkGetDeviceQueue(vk_device, m_gpuQueueFamilies.graphics, 0, &vk_graphicsQueue);
	vkGetDeviceQueue(vk_device, m_gpuQueueFamilies.present, 0, &vk_presentQueue);

	//Creating the swapchain that will own the framebuffers that will later be presented on screen
	VkSurfaceFormatKHR vk_surfaceFormat{};
	ChooseVulkanSurfaceFormat(vk_surfaceFormat, m_gpuSwapchainSupport.surfaceFormats);
	VkPresentModeKHR vk_swapchainPresentMode{};
	ChooseVulkanSwapchainPresentMode(vk_swapchainPresentMode, m_gpuSwapchainSupport.presentModes);
	VkExtent2D vk_swapchainExtent{};
	ChooseVulkanSwapchainExtent(vk_swapchainExtent, m_gpuSwapchainSupport.surfaceCapabilities,
		window.GetWidth(), window.GetHeight());
	vk_imageExtent = vk_swapchainExtent;
	vk_imageFormat = vk_surfaceFormat.format;
	VkSwapchainCreateInfoKHR vk_swapchainInfo{};
	CreateAppDefaultVkSwapchainInfo(vk_swapchainInfo, vk_surfaceFormat, vk_swapchainExtent, vk_swapchainPresentMode);
	CreateVulkanSwapchain(vk_swapchain, vk_device, vk_swapchainInfo);
	//After creating the swapchain, we retrieve the swapchain image handles from it
	uint32_t swapchainImageCount;
	vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &swapchainImageCount, nullptr);
	swapchainImages.resize(swapchainImageCount);
	vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &swapchainImageCount, swapchainImages.data());
	//Now that we have the swapchain images, we resize the image view array so that each image view correlates to a VkImage
	imageViews.resize(swapchainImages.size());

	/*Creating a base VkImageViewCreateInfo for the application that will be used to create the image views. The image views
	  will look into each image view that was retrieved from the swapchain*/
	VkImageViewCreateInfo vk_imageViewInfo{};
	vk_imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	vk_imageViewInfo.format = vk_imageFormat;
	vk_imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
	//Describes what the image's purpose is and which part of the image should be accessed
	vk_imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vk_imageViewInfo.subresourceRange.baseMipLevel = 0;
	vk_imageViewInfo.subresourceRange.levelCount = 1;
	vk_imageViewInfo.subresourceRange.baseArrayLayer = 0;
	vk_imageViewInfo.subresourceRange.layerCount = 1;
	//Allows for color channels to be swizzled around, stick to default
	vk_imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	vk_imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	vk_imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	vk_imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	for (uint32_t i = 0; i < swapchainImages.size(); ++i)
	{
		
		vk_imageViewInfo.image = swapchainImages[i];
		CreateVulkanSwapchainImageViews(imageViews[i], vk_imageViewInfo, vk_device);
	}

	//Creating the pipeline layout object to pass to the pipeline object later and to pass uniform variables when needed
	VkPipelineLayoutCreateInfo vk_pipelineLayoutInfo{};
	CreateAppDefaultPipelineLayoutInfo(vk_pipelineLayoutInfo);
	CreateVulkanGraphicsPipelineLayout(vk_pipelineLayoutInfo, vk_device, vk_pipelineLayout);

	//Creating the render pass that will later be passed into the graphics pipeline to specify the framebuffer attachments
	VkAttachmentDescription vk_attachmentInfo{};
	VkAttachmentReference vk_attachmentRef{};
	VkSubpassDescription vk_subpassInfo{};
	VkRenderPassCreateInfo vk_renderPassInfo{};
	VkSubpassDependency vk_dependencyInfo{};
	CreateAppDefaultRenderPassInfo(vk_renderPassInfo, vk_attachmentInfo, vk_attachmentRef, 
		vk_subpassInfo, vk_dependencyInfo);
	CreateVulkanRenderPass(vk_renderPass, vk_renderPassInfo, vk_device);

	//Reading the vertex and shader filenames
	const char* vertexShaderFilename = "Shaders/vert.spv";
	std::vector<char> vertexShaderCode;
	ReadShaderFile(vertexShaderCode, vertexShaderFilename);
	const char* fragShaderFilename = "Shaders/frag.spv";
	std::vector<char> fragShaderCode;
	ReadShaderFile(fragShaderCode, fragShaderFilename);
	//Creating the shader module wrappers that are needed to wrap around the shader code to be passed into the pipeline
	VkShaderModule vk_vertexShaderModule;
	VkShaderModule vk_fragShaderModule;
	VkPipelineShaderStageCreateInfo vk_vertexShaderStage{};
	VkPipelineShaderStageCreateInfo vk_fragShaderStage{};
	CreateShaderStages(vk_vertexShaderModule, vk_fragShaderModule, vk_vertexShaderStage, vk_fragShaderStage,
		vertexShaderCode, fragShaderCode, vk_device);
	VkPipelineShaderStageCreateInfo shaderStageInfos[] = { vk_vertexShaderStage, vk_fragShaderStage };
	VkPipelineShaderStageCreateInfo shaderStages[] = { vk_vertexShaderStage, vk_fragShaderStage };
	//Specifying the dynamic state of the pipeline
	std::vector<VkDynamicState> dynamicStates =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo vk_dynamicStateInfo{};
	vk_dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	vk_dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	vk_dynamicStateInfo.pDynamicStates = dynamicStates.data();
	//Creating all the infos for the pipeline fixed state
	VkPipelineInputAssemblyStateCreateInfo vk_inputAssemblyInfo{};
	VkPipelineViewportStateCreateInfo vk_viewportInfo{};
	VkPipelineRasterizationStateCreateInfo vk_rasterizationInfo{};
	VkPipelineMultisampleStateCreateInfo vk_multisamplingInfo{};
	VkPipelineColorBlendAttachmentState vk_colorBlendAttachment{};
	VkPipelineColorBlendStateCreateInfo vk_colorBlendInfo{};
	GraphicsPipelineFixedState pipelineFixedState;
	CreateAppDefaultPipelineFixedState(vk_inputAssemblyInfo, vk_viewportInfo, vk_rasterizationInfo, vk_multisamplingInfo,
		vk_colorBlendAttachment, vk_colorBlendInfo, pipelineFixedState);
	//Specifies the format of the vertex data that will be passed into the vertex shader
	VkPipelineVertexInputStateCreateInfo vk_vertexInputInfo{};
	CreateAppDefaultVkVertexInputInfo(vk_vertexInputInfo);
	//Creating the final pipeline info which will have pointer to all the other infos created for the pipeline
	VkGraphicsPipelineCreateInfo vk_pipelineInfo{};
	CreateAppDefaultVkPipelineInfo(vk_pipelineInfo, shaderStageInfos, vk_vertexInputInfo, pipelineFixedState,
		vk_dynamicStateInfo);
	CreateVulkanGraphicsPipeline(vk_graphicsPipeline, vk_pipelineInfo, vk_device);

	VkFramebufferCreateInfo vk_framebufferInfo{};
	framebuffers.resize(imageViews.size());
	for (uint32_t i = 0; i < framebuffers.size(); ++i)
	{
		CreateAppDefaultFramebufferInfo(vk_framebufferInfo, i);
		CreateVulkanFramebuffer(framebuffers[i], vk_framebufferInfo, vk_device);
	}

	//Creating the command pool before the command buffers so that we can allocate them
	VkCommandPoolCreateInfo vk_commandPoolInfo{};
	CreateAppDefaultVkCommandPoolInfo(vk_commandPoolInfo, m_gpuQueueFamilies.graphics);
	CreateVulkanCommandPool(vk_commandPool, vk_commandPoolInfo, vk_device);

	VkCommandBufferAllocateInfo vk_commandBufferInfo{};
	CreateAppDefaultVkCommandBufferInfo(vk_commandBufferInfo, vk_commandPool);
	AllocateVulkanCommandBuffer(vk_commandBuffer, vk_device, vk_commandBufferInfo);

	m_syncObjects.CreateSyncObjects(vk_device);
}

void VulkanGraphics::MainLoop()
{
	Draw();
}

void VulkanGraphics::Draw()
{

	//Wait for the previous frame to finish and reseting the fence when we get a signal from it
	vkWaitForFences(vk_device, 1, &m_syncObjects.vk_framesInFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(vk_device, 1, &m_syncObjects.vk_framesInFlightFence);

	//Getting the index of the next image that we can draw to
	uint32_t imageIndex;
	vkAcquireNextImageKHR(vk_device, vk_swapchain, UINT64_MAX, m_syncObjects.vk_imageAvailableSemaphore,
		VK_NULL_HANDLE, &imageIndex);
	
	//Resettig the command buffer for the previous frame
	vkResetCommandBuffer(vk_commandBuffer, 0);
	//Creating a begin info struct for the command buffer
	VkCommandBufferBeginInfo vk_commandBufferBegin{};
	CreateVulkanCommandBufferBeginInfo(vk_commandBufferBegin, 0, nullptr);
	//Creating a begin info struct for the render pass that we will be using
	VkRenderPassBeginInfo vk_renderPassBegin{};
	VkOffset2D vk_renderAreaOffset{ 0 , 0 };
	VkClearValue vk_clearValue{ {{0.0f, 0.0f, 0.0f, 1.0f}} };
	CreateVulkanRenderPassBeginInfo(vk_renderPassBegin, framebuffers[imageIndex], vk_renderPass, vk_imageExtent,
		vk_renderAreaOffset, 1, &vk_clearValue);
	//Recording the command buffer before submitting the queue
	RecordCommandBuffer(vk_commandBufferBegin, vk_renderPassBegin, vk_commandBuffer, vk_graphicsPipeline,
		vk_imageExtent);

	VkSubmitInfo vk_submitInfo{};
	//Specifies the operation we should wait to finish before submitting the queue
	VkSemaphore vk_waitSemaphores[] = { m_syncObjects.vk_imageAvailableSemaphore };
	//Signal the semaphore so that operation can continue after rendering is complete
	VkSemaphore vk_signalSemaphores[] = { m_syncObjects.vk_renderFinishedSemaphore };
	//Specifies in which stages of the pipeline the gpu should wait for the specified operations to finish
	VkPipelineStageFlags vk_pipelineWaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	CreateVulkanSubmitInfo(vk_submitInfo, 1, vk_waitSemaphores, vk_pipelineWaitStages, 1, vk_commandBuffer,
		1, vk_signalSemaphores);
	vkQueueSubmit(vk_graphicsQueue, 1, &vk_submitInfo, m_syncObjects.vk_framesInFlightFence);

	VkPresentInfoKHR vk_presentInfo{};
	VkSwapchainKHR vk_swapchains[] = { vk_swapchain };
	CreateVulkanPresentInfo(vk_presentInfo, 1, vk_signalSemaphores, 1, vk_swapchains, imageIndex);
	vkQueuePresentKHR(vk_presentQueue, &vk_presentInfo);
}

void VulkanGraphics::Cleanup()
{
	m_syncObjects.Cleanup(vk_device);
}


void VulkanGraphics::CreateAppDefaultVkInstanceInfo(VkInstanceCreateInfo& vk_instanceInfo, 
	VkApplicationInfo& vk_appInfo)
{
	//Initializing the application info struct that provides specialized info for the application to the instance info struct
	vk_appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vk_appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
	vk_appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	vk_appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	vk_appInfo.pApplicationName = "VulkanGraphics";
	vk_appInfo.pEngineName = "No Engine";

	//Create the instance info and passing the application info struct created above
	vk_instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vk_instanceInfo.pApplicationInfo = &vk_appInfo;

	//Validation layers
	vk_instanceInfo.enabledLayerCount = 0;
	vk_instanceInfo.ppEnabledLayerNames = nullptr;
}

void VulkanGraphics::CreateAppDefaultVkDeviceInfo(VkDeviceCreateInfo& vk_deviceInfo, 
	const QueueFamilyIndices& gpuQueueFamilyIndices,const std::vector<const char*>& requiredDeviceExtensions, 
	std::vector<VkDeviceQueueCreateInfo>& queueCreateInfos)
{
	/*Creating all the necessary queue create infos based on the queue family indices we retrieved from the graphics card
	  when we called the CheckGraphicsCardSwapchainSupport function from the PickPhysicalDevice function */
	std::set<uint32_t> uniqueQueueFamilies = { gpuQueueFamilyIndices.graphics,
			gpuQueueFamilyIndices.present };
	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	vk_deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	//Device layers
	vk_deviceInfo.enabledLayerCount = 0;

	//Device extensions
	vk_deviceInfo.enabledExtensionCount = requiredDeviceExtensions.size();
	vk_deviceInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

	//Device features
	vk_deviceInfo.pEnabledFeatures = nullptr;

	//Device queues
	vk_deviceInfo.queueCreateInfoCount = queueCreateInfos.size();
	vk_deviceInfo.pQueueCreateInfos = queueCreateInfos.data();
}

void VulkanGraphics::CreateAppDefaultVkSwapchainInfo(VkSwapchainCreateInfoKHR& vk_swapchainInfo,
	const VkSurfaceFormatKHR& vk_surfaceFormat, const VkExtent2D& vk_swapchainExtent,
	const VkPresentModeKHR& vk_presentMode)
{
	vk_swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	vk_swapchainInfo.surface = vk_surface;
	vk_swapchainInfo.presentMode = vk_presentMode;
	vk_swapchainInfo.imageColorSpace = vk_surfaceFormat.colorSpace;
	vk_swapchainInfo.imageFormat = vk_surfaceFormat.format;
	vk_swapchainInfo.imageExtent = vk_swapchainExtent;
	vk_swapchainInfo.preTransform = m_gpuSwapchainSupport.surfaceCapabilities.currentTransform;
	vk_swapchainInfo.clipped = VK_TRUE;
	vk_swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	vk_swapchainInfo.imageArrayLayers = 1;
	vk_swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = { m_gpuQueueFamilies.graphics, m_gpuQueueFamilies.present };
	if (m_gpuQueueFamilies.graphics != m_gpuQueueFamilies.present) {
		vk_swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		vk_swapchainInfo.queueFamilyIndexCount = 2;
		vk_swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		vk_swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vk_swapchainInfo.queueFamilyIndexCount = 0; // Optional
		vk_swapchainInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	vk_swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
}

void VulkanGraphics::CreateAppDefaultPipelineLayoutInfo(VkPipelineLayoutCreateInfo& vk_pipelineLayoutInfo)
{
	vk_pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	vk_pipelineLayoutInfo.setLayoutCount = 0;
	vk_pipelineLayoutInfo.pSetLayouts = nullptr;
	vk_pipelineLayoutInfo.pushConstantRangeCount = 0;
	vk_pipelineLayoutInfo.pPushConstantRanges = nullptr;
}

void VulkanGraphics::CreateAppDefaultRenderPassInfo(VkRenderPassCreateInfo& vk_renderPassInfo,
	VkAttachmentDescription& vk_attachmentInfo,VkAttachmentReference& vk_attachmentRef, 
	VkSubpassDescription& vk_subpassInfo, VkSubpassDependency& vk_subpassDependency)
{
	vk_attachmentInfo.format = vk_imageFormat;
	vk_attachmentInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	//For each new frame the framebuffer will be cleared to black before rendering
	vk_attachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	//The rendering results are stored so that they can be rendered to the screen
	vk_attachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	//The application does not use a stencil buffer
	vk_attachmentInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	vk_attachmentInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//The contents of the image from the previous will probably not be preserved (but it's going to be cleared either way)
	vk_attachmentInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	//The layout will be ready for presentation after the render pass is finished
	vk_attachmentInfo.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	/* One ore more subpasses can be passed into a single render pass. They consist of subsequent operations 
	   that depend on the contents of framebuffers from previous passes. One subpass can have multiple attachment 
	   references, which are references to one of the attachments like the one created above */

	//The attachment reference references the first attachment in the attachments array
	vk_attachmentRef.attachment = 0;
	//The attachment will be used as a color buffer 
	vk_attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//Explicitly stating that this is a graphics subpass
	vk_subpassInfo.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	//The subpass will have 1 attachment, the color attahment we created a reference for
	vk_subpassInfo.colorAttachmentCount = 1;
	vk_subpassInfo.pColorAttachments = &vk_attachmentRef;

	vk_subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	vk_subpassDependency.dstSubpass = 0;
	//Specifies which stage to wait on
	vk_subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	vk_subpassDependency.srcAccessMask = 0;
	vk_subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	vk_subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	vk_renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	vk_renderPassInfo.attachmentCount = 1;
	vk_renderPassInfo.pAttachments = &vk_attachmentInfo;
	vk_renderPassInfo.subpassCount = 1;
	vk_renderPassInfo.pSubpasses = &vk_subpassInfo;
	vk_renderPassInfo.dependencyCount = 1;
	vk_renderPassInfo.pDependencies = &vk_subpassDependency;
}

void VulkanGraphics::CreateAppDefaultPipelineFixedState(VkPipelineInputAssemblyStateCreateInfo& vk_inputAssemblyInfo,
	VkPipelineViewportStateCreateInfo& vk_viewportInfo, VkPipelineRasterizationStateCreateInfo& vk_rasterizationInfo,
	VkPipelineMultisampleStateCreateInfo& vk_multisamplingInfo, VkPipelineColorBlendAttachmentState& vk_colorBlendAttachment,
	VkPipelineColorBlendStateCreateInfo& vk_colorBlendInfo, GraphicsPipelineFixedState& fixedState)
{
	vk_inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	//Specifies what type of geometry will be drawn
	vk_inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	//Primitive restart allows breaking up lines and triangles
	vk_inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	//The viewport and the scissor will be specified as dynamic states, so we only need to pass their count
	vk_viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vk_viewportInfo.viewportCount = 1;
	vk_viewportInfo.scissorCount = 1;

	vk_rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	//Depth clamp is useful for shadow maps, we don't need it for this pipeline
	vk_rasterizationInfo.depthClampEnable = VK_FALSE;
	//This would disable the rasterizer, making presentation impossible
	vk_rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	vk_rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
	vk_rasterizationInfo.lineWidth = 1.0f;
	//Face culling settings
	vk_rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	vk_rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	//We won't be needing depth bias
	vk_rasterizationInfo.depthBiasEnable = VK_FALSE;

	/* Setting up multisampling which is a form of anti-alliasing. It combines the fragment shader results of multiple
	polygons that rasterize to the same pixel. This mainly occurs along edges,
	which is also where the most noticeable aliasing artifacts occur. It requires a GPU feature to enable. */
	vk_multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	vk_multisamplingInfo.sampleShadingEnable = VK_FALSE;
	vk_multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	//Setting up the depth and stencil buffers

	//Setting up color blending
	vk_colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	vk_colorBlendAttachment.blendEnable = VK_TRUE;
	vk_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	vk_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	vk_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	vk_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	vk_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	vk_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	vk_colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	vk_colorBlendInfo.logicOpEnable = VK_FALSE;
	vk_colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
	vk_colorBlendInfo.attachmentCount = 1;
	vk_colorBlendInfo.pAttachments = &vk_colorBlendAttachment;
	vk_colorBlendInfo.blendConstants[0] = 0.0f;
	vk_colorBlendInfo.blendConstants[1] = 0.0f;
	vk_colorBlendInfo.blendConstants[2] = 0.0f;
	vk_colorBlendInfo.blendConstants[3] = 0.0f;

	fixedState.vk_viewportInfo = vk_viewportInfo;
	fixedState.vk_inputAssemblyInfo = vk_inputAssemblyInfo;
	fixedState.vk_rasterizationInfo = vk_rasterizationInfo;
	fixedState.vk_multisamplingInfo = vk_multisamplingInfo;
	fixedState.vk_colorBlendInfo = vk_colorBlendInfo;
}

void VulkanGraphics::ReadShaderFile(std::vector<char>& shaderCode, const char* shaderFilename)
{
	std::ifstream shaderFile(shaderFilename, std::ios::ate | std::ios::binary);
	if (!shaderFile.is_open())
	{
		__debugbreak();
	}

	size_t filesize = static_cast<size_t>(shaderFile.tellg());
	shaderCode.resize(filesize);

	shaderFile.seekg(0);
	shaderFile.read(shaderCode.data(), filesize);

	shaderFile.close();
}

void VulkanGraphics::CreateShaderStages(VkShaderModule& vk_vertexShaderModule, VkShaderModule& vk_fragShaderModule,
	VkPipelineShaderStageCreateInfo& vk_vertexShaderStageInfo, VkPipelineShaderStageCreateInfo& vk_fragShaderStageInfo,
	std::vector<char>& vertexShaderCode, std::vector<char>& fragShaderCode, const VkDevice& vk_device)
{
	VkShaderModuleCreateInfo vk_vertexShaderModuleInfo{};
	vk_vertexShaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vk_vertexShaderModuleInfo.codeSize = vertexShaderCode.size();
	vk_vertexShaderModuleInfo.pCode = reinterpret_cast<uint32_t*>(vertexShaderCode.data());
	VkResult vk_vertexShaderModuleCreationResult = vkCreateShaderModule(vk_device, &vk_vertexShaderModuleInfo,
		nullptr, &vk_vertexShaderModule);
	if (vk_vertexShaderModuleCreationResult != VK_SUCCESS)
	{
		__debugbreak();
	}

	VkShaderModuleCreateInfo vk_fragShaderModuleInfo{};
	vk_fragShaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vk_fragShaderModuleInfo.codeSize = fragShaderCode.size();
	vk_fragShaderModuleInfo.pCode = reinterpret_cast<uint32_t*>(fragShaderCode.data());
	VkResult vk_fragShaderModuleCreationResult = vkCreateShaderModule(vk_device, &vk_fragShaderModuleInfo,
		nullptr, &vk_fragShaderModule);
	if (vk_fragShaderModuleCreationResult != VK_SUCCESS)
	{
		__debugbreak();
	}
	
	vk_vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vk_vertexShaderStageInfo.module = vk_vertexShaderModule;
	vk_vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vk_vertexShaderStageInfo.pName = "main";

	vk_fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vk_fragShaderStageInfo.module = vk_fragShaderModule;
	vk_fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	vk_fragShaderStageInfo.pName = "main";
}

void VulkanGraphics::CreateAppDefaultVkPipelineInfo(VkGraphicsPipelineCreateInfo& vk_pipelineInfo,
	const VkPipelineShaderStageCreateInfo* shaderStageInfos, const VkPipelineVertexInputStateCreateInfo& vk_vertexInputStateInfo,
	const GraphicsPipelineFixedState& fixedState, const VkPipelineDynamicStateCreateInfo& vk_dynamicStateInfo)
{
	vk_pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	vk_pipelineInfo.stageCount = 2;
	vk_pipelineInfo.pStages = shaderStageInfos;
	vk_pipelineInfo.pDynamicState = &vk_dynamicStateInfo;
	vk_pipelineInfo.pInputAssemblyState = &fixedState.vk_inputAssemblyInfo;
	vk_pipelineInfo.pMultisampleState = &fixedState.vk_multisamplingInfo;
	vk_pipelineInfo.pColorBlendState = &fixedState.vk_colorBlendInfo;
	vk_pipelineInfo.pViewportState = &fixedState.vk_viewportInfo;
	vk_pipelineInfo.pDepthStencilState = nullptr;
	vk_pipelineInfo.pRasterizationState = &fixedState.vk_rasterizationInfo;
	vk_pipelineInfo.pVertexInputState = &vk_vertexInputStateInfo;
	vk_pipelineInfo.subpass = 0;
	vk_pipelineInfo.renderPass = vk_renderPass;
	vk_pipelineInfo.layout = vk_pipelineLayout;
	vk_pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

}

void VulkanGraphics::CreateAppDefaultVkVertexInputInfo(VkPipelineVertexInputStateCreateInfo& vk_vertexInputInfo)
{
	vk_vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vk_vertexInputInfo.vertexBindingDescriptionCount = 0;
}

void VulkanGraphics::CreateAppDefaultFramebufferInfo(VkFramebufferCreateInfo& vk_framebufferInfo, 
	uint32_t imageViewIndex)
{
	vk_framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	vk_framebufferInfo.width = vk_imageExtent.width;
	vk_framebufferInfo.height = vk_imageExtent.height;
	vk_framebufferInfo.renderPass = vk_renderPass;
	vk_framebufferInfo.layers = 1;
	vk_framebufferInfo.attachmentCount = 1;
	vk_framebufferInfo.pAttachments = &imageViews[imageViewIndex];
}

void VulkanGraphics::CreateAppDefaultVkCommandPoolInfo(VkCommandPoolCreateInfo& vk_commandPoolInfo,
	uint32_t graphicsQueueFamilyIndex)
{
	vk_commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	//We will be recording a command buffer every frame, so we want to be able to reset and rerecord over it
	vk_commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	//The command pool will be used to allocate, a graphics command buffer
	vk_commandPoolInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
}

void VulkanGraphics::CreateAppDefaultVkCommandBufferInfo(VkCommandBufferAllocateInfo& vk_commandBufferInfo,
	const VkCommandPool& vk_commandPool)
{
	vk_commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	//This command buffer can be provided to a queue for excecution directly, but cannot be called by other command buffers
	vk_commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	vk_commandBufferInfo.commandPool = vk_commandPool;
	vk_commandBufferInfo.commandBufferCount = 1;
}