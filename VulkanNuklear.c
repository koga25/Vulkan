#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_VULKAN_IMPLEMENTATION


#include "VulkanNuklear.h"
#include "Vulkan.h"


#define INCLUDE_OVERVIEW

struct VertexHelper nkVertexHelper;
struct nk_context* nkContext;
struct nk_color background;
struct nk_font_atlas* atlas;
struct nk_vec2 scroll;
unsigned int text[NK_GLFW_TEXT_MAX];
int textLength;
double lastButtonClick;
int isDoubleClick;
struct nk_vec2 doubleClickPosition;
int windowWidth;
int windowHeight;
int displayWidth;
int displayHeight;
struct nk_vec2 fbScale;
struct nk_buffer nkCmd;

bool doWeRotate;

//called from Vulkan.c InitVulkan function
void InitNuklear()
{
	NuklearInstallCallbacks();
	NuklearGetContext();

	CopyDuplicates();
	NuklearCreateDescriptorPool();
	NuklearCreateDescriptorSetLayout();
	NuklearCreateDescriptorSets();
	NuklearCreateCommandPool();
	NuklearCreateCommandBuffers();
	NuklearCreateSyncObjects();
	NuklearCreateRenderPass();
	NuklearCreateBuffer(MAX_VERTEX_BUFFER, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &nkVulkan.vertexBuffer, &nkVulkan.vertexBufferMemory);
	NuklearCreateBuffer(MAX_INDEX_BUFFER, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &nkVulkan.indexBuffer, &nkVulkan.indexBufferMemory);
	NuklearCreateBuffer(sizeof(struct Mat4f), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &nkVulkan.uniformBuffers, &nkVulkan.uniformBuffersMemory);
	NuklearCreateGraphicsPipeline();

	//here it will start the nuklear atlas and create the texture image
	atlas = (struct nk_font_atlas*)malloc(sizeof(struct nk_font_atlas));
	nk_glfw3_font_stash_begin(atlas);
	nk_glfw3_font_stash_end();

	nkSettings.bg_color[0] = 0.0f;
	nkSettings.bg_color[1] = 0.0f;
	nkSettings.bg_color[2] = 0.0f;
	nkSettings.bg_color[3] = 1.0f;
	nkSettings.orientation = UP;
	nkSettings.zoom = 20;
}

void NuklearInstallCallbacks()
{
	glfwSetScrollCallback(window, nk_gflw3_scroll_callback);
	glfwSetCharCallback(window, nk_glfw3_char_callback);
	glfwSetMouseButtonCallback(window, nk_glfw3_mouse_button_callback);
}

NK_API void nk_gflw3_scroll_callback(GLFWwindow* win, double xoff, double yoff)
{
	(void)win; (void)xoff;
	scroll.x += (float)xoff;
	scroll.y += (float)yoff;
}

NK_API void nk_glfw3_char_callback(GLFWwindow* win, unsigned int codepoint)
{
	(void)win;
	if (textLength < NK_GLFW_TEXT_MAX)
	{
		text[textLength++] = codepoint;
	}
}

NK_API void nk_glfw3_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double x, y;
	if (button != GLFW_MOUSE_BUTTON_LEFT)
	{
		return;
	}

	glfwGetCursorPos(window, &x, &y);

	if (action == GLFW_PRESS) 
	{
		double dt = glfwGetTime() - lastButtonClick;
		if (dt > NK_GLFW_DOUBLE_CLICK_LO && dt < NK_GLFW_DOUBLE_CLICK_HI) 
		{
			isDoubleClick = nk_true;
			doubleClickPosition = nk_vec2((float)x, (float)y);
		}
		lastButtonClick = glfwGetTime();
	}
	else
	{
		isDoubleClick = nk_false;
	}
}

void NuklearGetContext()
{
	nkContext = (struct nk_context*)malloc(sizeof(struct nk_context));
	nk_init_default(nkContext, 0);
	nkContext->clip.copy = nk_glfw3_clipboard_copy;
	nkContext->clip.paste = nk_glfw3_clipboard_paste;
	nkContext->clip.userdata = nk_handle_ptr(0);
	lastButtonClick = 0;
	isDoubleClick = nk_false;
	doubleClickPosition = nk_vec2(0, 0);
	nk_buffer_init_default(&nkCmd);
}

NK_INTERN void nk_glfw3_clipboard_copy(nk_handle user, const char* text, int length)
{
	char* str = 0;
	(void)user;
	if (!length)
	{
		return;
	}
	str = (char*)malloc((size_t)length + 1);
	if (!str)
	{
		return;
	}
	memcpy(str, text, (size_t)length);
	str[length] = '\0';
	glfwSetClipboardString(window, str);
	free(str);
}

NK_INTERN void nk_glfw3_clipboard_paste(nk_handle user, struct nk_text_edit* edit)
{
	const char* text = glfwGetClipboardString(window);
	if (text)
	{
		nk_textedit_paste(edit, text, nk_strlen(text));
	}
	(void)user;
}

void CopyDuplicates()
{
	nkVulkan.nuklearFamilyIndices.graphicsFamily = context.familyIndices.graphicsFamily;
	nkVulkan.nuklearFamilyIndices.graphicsFamilyFound = context.familyIndices.graphicsFamilyFound;
	nkVulkan.nuklearFamilyIndices.presentFamily = context.familyIndices.presentFamily;
	nkVulkan.nuklearFamilyIndices.presentFamilyFound = context.familyIndices.presentFamilyFound;
	nkVulkan.physicalDevice = context.physicalDevice;
	nkVulkan.physicalDeviceProperties = context.physicalDeviceProperties;
	nkVulkan.physicalDeviceMemoryProperties = context.physicalDeviceMemoryProperties;
	nkVulkan.logicalDevice = context.logicalDevice;
	nkVulkan.nuklearSwapChainDetails.capabilities = context.swapChainDetails.capabilities;
	nkVulkan.nuklearSwapChainDetails.formats = context.swapChainDetails.formats;
	nkVulkan.nuklearSwapChainDetails.formatsLength = context.swapChainDetails.formatsLength;
	nkVulkan.nuklearSwapChainDetails.presentModes = context.swapChainDetails.presentModes;
	nkVulkan.nuklearSwapChainDetails.presentModesLength = context.swapChainDetails.presentModesLength;
	nkVulkan.swapChain = context.swapChain;
	nkVulkan.swapChainImages = context.swapChainImages;
	nkVulkan.swapChainImagesCount = nkVulkan.swapChainImagesCount;
	nkVulkan.swapChainImageFormat = context.swapChainImageFormat;
	nkVulkan.swapChainExtent = context.swapChainExtent;
	nkVulkan.swapChainImageViews = context.swapChainImageViews;
	nkVulkan.graphicsQueue = context.graphicsQueue;

	nkVulkan.swapChainFrameBuffers = context.swapChainFrameBuffers;
}

void NuklearCreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding = { 0 };
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding = { 0 };
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = NULL;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	uint32_t sizeOfBindings = 2;

	VkDescriptorSetLayoutBinding bindings[2] = { uboLayoutBinding, samplerLayoutBinding };


	VkDescriptorSetLayoutCreateInfo layoutInfo = { 0 };
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = sizeOfBindings;
	layoutInfo.pBindings = bindings;

	CheckVulkanResult(vkCreateDescriptorSetLayout(nkVulkan.logicalDevice, &layoutInfo, NULL, &nkVulkan.descriptorSetLayout), "Failed to create descriptor set layout");
}

void NuklearCreateDescriptorPool()
{
	uint32_t poolSizeCount = 2;
	VkDescriptorPoolSize poolSize[2] = { 0 };
	poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize[0].descriptorCount = 1;
	poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize[1].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo = { 0 };
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = poolSizeCount;
	poolInfo.pPoolSizes = poolSize;
	poolInfo.maxSets = 1;

	CheckVulkanResult(vkCreateDescriptorPool(nkVulkan.logicalDevice, &poolInfo, NULL, &nkVulkan.descriptorPool), "failed to create nuklear descriptor pool");

}

void NuklearCreateDescriptorSets()
{
	VkDescriptorSetAllocateInfo allocInfo = { 0 };
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = nkVulkan.descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &nkVulkan.descriptorSetLayout;

	CheckVulkanResult(vkAllocateDescriptorSets(nkVulkan.logicalDevice, &allocInfo, &nkVulkan.descriptorSets), "failed to allocate nuklear descriptor sets");
}

static VkVertexInputBindingDescription NuklearGetBindingDescription() {
	VkVertexInputBindingDescription bindingDescription = { 0 };
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(struct nkVertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	nkVertexHelper.vulkanBindingDescriptionSize = 1;

	return bindingDescription;
}

static VkVertexInputAttributeDescription* NuklearGetAttributeDescriptions() {
	VkVertexInputAttributeDescription* attributeDescriptions = (VkVertexInputAttributeDescription*)malloc(sizeof(VkVertexInputAttributeDescription) * 3);
	if (attributeDescriptions == NULL)
	{
		printf("failed to allocate attributeDescriptions");
		exit(1);
	}

	nkVertexHelper.vulkanAttributeDescriptionSize = 3;

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[0].offset = NK_OFFSETOF(struct nkVertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[1].offset = NK_OFFSETOF(struct nkVertex, uv);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R8G8B8A8_UINT;
	attributeDescriptions[2].offset = NK_OFFSETOF(struct nkVertex, col);

	return attributeDescriptions;
}

void NuklearCreateCommandPool()
{
	VkCommandPoolCreateInfo poolInfo = { 0 };
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = nkVulkan.nuklearFamilyIndices.graphicsFamily;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	CheckVulkanResult(vkCreateCommandPool(nkVulkan.logicalDevice, &poolInfo, NULL, &nkVulkan.commandPool), "failed to create command pool");
}

void NuklearCreateCommandBuffers()
{
	VkCommandBufferAllocateInfo allocInfo = { 0 };
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = nkVulkan.commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;


	nkVulkan.commandBuffers = (VkCommandBuffer*)malloc(sizeof(VkCommandBuffer));

	CheckVulkanResult(vkAllocateCommandBuffers(nkVulkan.logicalDevice, &allocInfo, nkVulkan.commandBuffers), "couldn't allocade nuklear command buffers");
}

void NuklearCreateSyncObjects()
{
	VkSemaphoreCreateInfo createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	CheckVulkanResult(vkCreateSemaphore(nkVulkan.logicalDevice, &createInfo, NULL, &nkVulkan.renderCompleted), "couldn't create nuklear semaphore");
}

void NuklearCreateRenderPass()
{
	VkAttachmentDescription colorAttachment = { 0 };
	colorAttachment.format = nkVulkan.swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentReference = { 0 };
	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = { 0 };
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentReference;

	VkSubpassDependency dependency = { 0 };
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo = { 0 };
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	CheckVulkanResult(vkCreateRenderPass(nkVulkan.logicalDevice, &renderPassInfo, NULL, &nkVulkan.renderPass), "failed to create nuklear render pass");
	
}

void NuklearCreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
{
	VkBufferCreateInfo bufferInfo = { 0 };
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	CheckVulkanResult(vkCreateBuffer(context.logicalDevice, &bufferInfo, NULL, &*buffer), "failed to create vertex buffer");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(context.logicalDevice, *buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = { 0 };
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	CheckVulkanResult(vkAllocateMemory(context.logicalDevice, &allocInfo, NULL, &*bufferMemory), "failed to allocate vertex buffer memory");

	vkBindBufferMemory(context.logicalDevice, *buffer, *bufferMemory, 0);
}

NK_API void nk_glfw3_font_stash_begin(struct nk_font_atlas* atlas)
{
	
	nk_font_atlas_init_default(atlas);
	nk_font_atlas_begin(atlas);
}

NK_API void nk_glfw3_font_stash_end()
{
	const void* image;
	int width;
	int height;

	image = nk_font_atlas_bake(atlas, &width, &height, NK_FONT_ATLAS_RGBA32);
	NuklearCreateTextureImage(image, width, height);
	nk_font_atlas_end(atlas, nk_handle_ptr(nkVulkan.fontSampler), &nkVulkan.nullTexture);
	if (atlas->default_font)
	{
		nk_style_set_font(nkContext, &atlas->default_font->handle);
	}

	NuklearUpdateDescriptorSets();
}

void NuklearCreateGraphicsPipeline()
{
	char* vertShaderCode = ReadFile("shaders/Nuklear/vert.spv", &nkVulkan.nuklearVertFragBufferDetails.vertexShaderBufferLength);
	char* fragShaderCode = ReadFile("shaders/Nuklear/frag.spv", &nkVulkan.nuklearVertFragBufferDetails.fragmentShaderBufferLength);

	VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode, nkVulkan.nuklearVertFragBufferDetails.vertexShaderBufferLength);
	VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode, nkVulkan.nuklearVertFragBufferDetails.fragmentShaderBufferLength);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = { 0 };
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = { 0 };
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = { 0 };
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	VkVertexInputBindingDescription bindingDescription = NuklearGetBindingDescription();
	vertexInputInfo.vertexBindingDescriptionCount = (uint32_t)nkVertexHelper.vulkanBindingDescriptionSize;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	VkVertexInputAttributeDescription* attributeDescriptions = NuklearGetAttributeDescriptions();
	vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)nkVertexHelper.vulkanAttributeDescriptionSize;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = { 0 };
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;


	VkPipelineViewportStateCreateInfo viewportState = { 0 };
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer = { 0 };
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

	VkPipelineMultisampleStateCreateInfo multisampling = { 0 };
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = { 0 };
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_MASK_RGBA;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = { 0 };
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;

	VkPipelineDepthStencilStateCreateInfo depthStencil = { 0 };
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

	VkDynamicState dynamicStates[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = { 0 };
	dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.pDynamicStates = dynamicStates;
	dynamicStateCreateInfo.dynamicStateCount = 2;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = { 0 };
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &nkVulkan.descriptorSetLayout;

	CheckVulkanResult(vkCreatePipelineLayout(nkVulkan.logicalDevice, &pipelineLayoutInfo, NULL, &nkVulkan.pipelineLayout), "failed to create pipeline layout");

	VkGraphicsPipelineCreateInfo pipelineInfo = { 0 };
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicStateCreateInfo;
	pipelineInfo.layout = nkVulkan.pipelineLayout;
	pipelineInfo.renderPass = nkVulkan.renderPass;
	pipelineInfo.basePipelineIndex = -1;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	CheckVulkanResult(vkCreateGraphicsPipelines(nkVulkan.logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &nkVulkan.graphicsPipeline), "failed to create graphics pipeline");
	vkDestroyShaderModule(nkVulkan.logicalDevice, vertShaderModule, NULL);
	vkDestroyShaderModule(nkVulkan.logicalDevice, fragShaderModule, NULL);
}

void NuklearCreateTextureImage(const void* image, int width, int height)
{
	VkImageCreateInfo imageInfo = { 0 };
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	CheckVulkanResult(vkCreateImage(context.logicalDevice, &imageInfo, NULL, &nkVulkan.fontImage), "Failed to create image");
	
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(context.logicalDevice, nkVulkan.fontImage, &memRequirements);

	VkMemoryAllocateInfo allocInfo = { 0 };
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	CheckVulkanResult(vkAllocateMemory(context.logicalDevice, &allocInfo, NULL, &nkVulkan.textureImageMemory), "failed to allocate image memory");

	vkBindImageMemory(nkVulkan.logicalDevice, nkVulkan.fontImage, nkVulkan.textureImageMemory, 0);

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	/*CreateBuffer(allocInfo.allocationSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&stagingBuffer, &stagingBufferMemory);*/

	VkBufferCreateInfo bufferInfo = { 0 };
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = allocInfo.allocationSize;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	CheckVulkanResult(vkCreateBuffer(nkVulkan.logicalDevice, &bufferInfo, NULL, &stagingBuffer), "couldn't create staging buffer");
	vkGetBufferMemoryRequirements(nkVulkan.logicalDevice, stagingBuffer, &memRequirements);

	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	CheckVulkanResult(vkAllocateMemory(nkVulkan.logicalDevice, &allocInfo, NULL, &stagingBufferMemory),"Couldn't allocate buffer memory");
	CheckVulkanResult(vkBindBufferMemory(nkVulkan.logicalDevice, stagingBuffer, stagingBufferMemory, 0), "Couldn't bind buffer memory");

	void* data;
	CheckVulkanResult(vkMapMemory(context.logicalDevice, stagingBufferMemory, 0, allocInfo.allocationSize, 0, &data), "couldn't map memory");
	memcpy(data, image, (size_t)(width * height * 4));
	vkUnmapMemory(context.logicalDevice, stagingBufferMemory);

	VkCommandBufferBeginInfo beginInfo = { 0 };
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VkCommandBuffer commandBuffer = nkVulkan.commandBuffers[0];
	CheckVulkanResult(vkBeginCommandBuffer(commandBuffer, &beginInfo), "couldn't begin command buffer");

	VkImageMemoryBarrier imageMemoryBarrier = { 0 };
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.image = nkVulkan.fontImage;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageMemoryBarrier.subresourceRange.levelCount = 1;
	imageMemoryBarrier.subresourceRange.layerCount = 1;
	imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);

	VkBufferImageCopy bufferCopyRegion = { 0 };
	bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	bufferCopyRegion.imageSubresource.mipLevel = 0;
	bufferCopyRegion.imageSubresource.layerCount = 1;
	bufferCopyRegion.imageExtent.width = width;
	bufferCopyRegion.imageExtent.height = height;
	bufferCopyRegion.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, nkVulkan.fontImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferCopyRegion);

	VkImageMemoryBarrier imageShaderMemoryBarrier = { 0 };
	imageShaderMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageShaderMemoryBarrier.image = nkVulkan.fontImage;
	imageShaderMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageShaderMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageShaderMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageShaderMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageShaderMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageShaderMemoryBarrier.subresourceRange.levelCount = 1;
	imageShaderMemoryBarrier.subresourceRange.layerCount = 1;
	imageShaderMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imageShaderMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &imageShaderMemoryBarrier);

	CheckVulkanResult(vkEndCommandBuffer(commandBuffer), "Couldn't end command buffer");

	VkSubmitInfo submitInfo = { 0 };
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	CheckVulkanResult(vkQueueSubmit(nkVulkan.graphicsQueue, 1, &submitInfo, NULL), "couldn't submit queue");
	CheckVulkanResult(vkQueueWaitIdle(nkVulkan.graphicsQueue), "couldn't wait queue idle");

	vkFreeMemory(nkVulkan.logicalDevice, stagingBufferMemory, NULL);
	vkDestroyBuffer(nkVulkan.logicalDevice, stagingBuffer, NULL);

	VkImageViewCreateInfo imageViewInfo = { 0 };
	imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewInfo.image = nkVulkan.fontImage;
	imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewInfo.format = imageInfo.format;
	imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewInfo.subresourceRange.baseArrayLayer = 0;
	//mip level was 1
	imageViewInfo.subresourceRange.baseMipLevel = 0;
	//layer count was 0
	imageViewInfo.subresourceRange.layerCount = 1;
	imageViewInfo.subresourceRange.levelCount = 1;

	CheckVulkanResult(vkCreateImageView(nkVulkan.logicalDevice, &imageViewInfo, NULL, &nkVulkan.fontImageView), "Couldn't create image view");

	VkSamplerCreateInfo samplerInfo = { 0 };
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.maxAnisotropy = 1.0;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;
	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

	CheckVulkanResult(vkCreateSampler(nkVulkan.logicalDevice, &samplerInfo, NULL, &nkVulkan.fontSampler), "couldn't create sampler");
}

void NuklearUpdateDescriptorSets()
{
	VkDescriptorBufferInfo bufferInfo = { 0 };
	bufferInfo.buffer = nkVulkan.uniformBuffers;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(struct Mat4f);

	VkDescriptorImageInfo imageInfo = { 0 };
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = nkVulkan.fontImageView;
	imageInfo.sampler = nkVulkan.fontSampler;

	VkWriteDescriptorSet descriptorWrites[2];
	memset(descriptorWrites, 0, sizeof(VkWriteDescriptorSet) * 2);
	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = nkVulkan.descriptorSets;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	//if the font sampler is null, we need to write only 1 descriptor set so we will check if it isnt and increment the count.
	uint32_t descriptorWriteCount = 1;
	if (nkVulkan.fontSampler != NULL)
	{
		descriptorWriteCount++;
		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = nkVulkan.descriptorSets;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;
	}

	vkUpdateDescriptorSets(nkVulkan.logicalDevice, descriptorWriteCount, descriptorWrites, 0, NULL);
}

//called from Vulkan.c DrawFrame function
VkSemaphore SubmitOverlay(struct overlaySettings* settings, uint32_t bufferIndex, VkSemaphore mainFinishedSemaphore)
{
	struct nk_color background = nk_rgb((int)(nkSettings.bg_color[0] * 255), (int)(nkSettings.bg_color[1] * 255), (int)(nkSettings.bg_color[2] * 255));

	//input
	glfwPollEvents();
	nk_glfw3_new_frame();

	//gui
	if (nk_begin(nkContext, "Demo", nk_rect(50, 50, 230, 250), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
	{
		nk_layout_row_static(nkContext, 30, 80, 1);
		if (nk_button_label(nkContext, "button"))
		{
			doWeRotate = !doWeRotate;
			printf("button pressed\n");
		}

		nk_layout_row_dynamic(nkContext, 30, 2);
		if (nk_option_label(nkContext, "up", settings->orientation == UP))
		{
			settings->orientation = UP;
		}
		if (nk_option_label(nkContext, "down", settings->orientation == DOWN))
		{
			settings->orientation = DOWN;
		}

		nk_layout_row_dynamic(nkContext, 25, 1);
		nk_property_int(nkContext, "Zoom:", 0, &settings->zoom, 100, 10, 1);

		nk_layout_row_dynamic(nkContext, 20, 1);
		nk_label(nkContext, "background:", NK_TEXT_LEFT);
		nk_layout_row_dynamic(nkContext, 25, 1);
		if (nk_combo_begin_color(nkContext, background, nk_vec2(nk_widget_width(nkContext), 400)))
		{
			nk_layout_row_dynamic(nkContext, 120, 1);
			background = nk_rgba_cf(nk_color_picker(nkContext, nk_color_cf(background), NK_RGBA));
			nk_layout_row_dynamic(nkContext, 25, 1);
			background.r = nk_propertyf(nkContext, "#R:", 0, background.r, 255, 1, 1);
			background.g = nk_propertyf(nkContext, "#G:", 0, background.g, 255, 1, 1);
			background.b = nk_propertyf(nkContext, "#B:", 0, background.b, 255, 1, 1);
			background.a = nk_propertyf(nkContext, "#A:", 0, background.a, 255, 1, 1);
			nk_combo_end(nkContext);
		}
	}
	nk_end(nkContext);

	return nk_glfw3_render(NK_ANTI_ALIASING_ON, bufferIndex, mainFinishedSemaphore);
}

NK_API void nk_glfw3_new_frame()
{
	int i;
	double x;
	double y;

	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
	fbScale.x = (float)displayWidth / (float)windowWidth;
	fbScale.y = (float)displayHeight / windowHeight;

	nk_input_begin(nkContext);
	for (i = 0; i < textLength; i++)
	{
		nk_input_unicode(nkContext, text[i]);
	}

#ifdef NK_GLFW_GL3_MOUSE_GRABBING
	if (nkContext->input.mouse.grab)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	else if (nkContext->input.mouse.ungrab)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
#endif // NK_GLFW_GL3_MOUSE_GRABBING

	nk_input_key(nkContext, NK_KEY_DEL, glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS);
	nk_input_key(nkContext, NK_KEY_ENTER, glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS);
	nk_input_key(nkContext, NK_KEY_TAB, glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS);
	nk_input_key(nkContext, NK_KEY_BACKSPACE, glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS);
	nk_input_key(nkContext, NK_KEY_UP, glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS);
	nk_input_key(nkContext, NK_KEY_DOWN, glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS);
	nk_input_key(nkContext, NK_KEY_TEXT_START, glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS);
	nk_input_key(nkContext, NK_KEY_SCROLL_START, glfwGetKey(window, GLFW_KEY_END) == GLFW_PRESS);
	nk_input_key(nkContext, NK_KEY_SCROLL_END, glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS);
	nk_input_key(nkContext, NK_KEY_SCROLL_DOWN, glfwGetKey(window, GLFW_KEY_END) == GLFW_PRESS);
	nk_input_key(nkContext, NK_KEY_SCROLL_UP, glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS);
	nk_input_key(nkContext, NK_KEY_SHIFT, glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT));

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL))
	{
		nk_input_key(nkContext, NK_KEY_COPY, glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS);
		nk_input_key(nkContext, NK_KEY_PASTE, glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS);
		nk_input_key(nkContext, NK_KEY_CUT, glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS);
		nk_input_key(nkContext, NK_KEY_TEXT_UNDO, glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS);
		nk_input_key(nkContext, NK_KEY_TEXT_REDO, glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS);
		nk_input_key(nkContext, NK_KEY_TEXT_WORD_LEFT, glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
		nk_input_key(nkContext, NK_KEY_TEXT_WORD_RIGHT, glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);
		nk_input_key(nkContext, NK_KEY_TEXT_LINE_START, glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS);
		nk_input_key(nkContext, NK_KEY_TEXT_LINE_END, glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS);
	}
	else
	{
		nk_input_key(nkContext, NK_KEY_LEFT, glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS);
		nk_input_key(nkContext, NK_KEY_RIGHT, glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS);
		nk_input_key(nkContext, NK_KEY_COPY, 0);
		nk_input_key(nkContext, NK_KEY_PASTE, 0);
		nk_input_key(nkContext, NK_KEY_CUT, 0);
		nk_input_key(nkContext, NK_KEY_SHIFT, 0);
	}

	glfwGetCursorPos(window, &x, &y);
	nk_input_motion(nkContext, (int)x, (int)y);

#ifdef NK_GLFW_GL3_MOUSE_GRABBING
	if (nkContext->input.mouse.grabbed)
	{
		glfwSetCursorPos(window, nkContext->input.mouse.prev.x, nkContext->input.mouse.prev.y);
		nkContext->input.mouse.pos.x = nkContext->input.mouse.prev.x;
		nkContext->input.mouse.pos.y = nkContext->input.mouse.prev.y;
	}
#endif // NK_GLFW_GL3_MOUSE_GRABBING

	nk_input_button(nkContext, NK_BUTTON_LEFT, (int)x, (int)y, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
	nk_input_button(nkContext, NK_BUTTON_MIDDLE, (int)x, (int)y, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
	nk_input_button(nkContext, NK_BUTTON_RIGHT, (int)x, (int)y, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
	nk_input_button(nkContext, NK_BUTTON_DOUBLE, (int)doubleClickPosition.x, (int)doubleClickPosition.y, isDoubleClick);
	nk_input_scroll(nkContext, scroll);
	nk_input_end(nkContext);
	textLength = 0;
	scroll = nk_vec2(0, 0);
}

NK_API VkSemaphore nk_glfw3_render(enum nk_anti_aliasing AA, uint32_t bufferIndex, VkSemaphore waitSemaphore)
{
	struct nk_buffer verticesBuffer;
	struct nk_buffer elementsBuffer;

	struct Mat4f projection =
	{
		2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f
	};

	projection.m[0] /= windowWidth;
	projection.m[5] /= windowHeight;

	void* data;
	vkMapMemory(nkVulkan.logicalDevice, nkVulkan.uniformBuffersMemory, 0, sizeof(projection), 0, &data);
	memcpy(data, &projection, sizeof(projection));
	vkUnmapMemory(nkVulkan.logicalDevice, nkVulkan.uniformBuffersMemory);

	VkCommandBufferBeginInfo beginInfo = { 0 };
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VkRenderPassBeginInfo renderPassBeginInfo = { 0 };
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = nkVulkan.renderPass;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent.width = displayWidth;
	renderPassBeginInfo.renderArea.extent.height = displayHeight;
	renderPassBeginInfo.clearValueCount = 0;
	renderPassBeginInfo.pClearValues = NULL;
	renderPassBeginInfo.framebuffer = nkVulkan.swapChainFrameBuffers[bufferIndex];

	VkCommandBuffer commandBuffer = nkVulkan.commandBuffers[0];

	CheckVulkanResult(vkBeginCommandBuffer(commandBuffer, &beginInfo), "couldn't begin nuklear command buffer");
	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport = { 0 };
	viewport.width = (float)displayWidth;
	viewport.height = (float)displayHeight;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, nkVulkan.graphicsPipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, nkVulkan.pipelineLayout, 0, 1, &nkVulkan.descriptorSets, 0, NULL);
	{
		const struct nk_draw_command* cmd;
		void* vertices;
		void* elements;
		nk_draw_index offset = 0;

		vkMapMemory(nkVulkan.logicalDevice, nkVulkan.vertexBufferMemory, 0, MAX_VERTEX_BUFFER, 0, &vertices);
		vkMapMemory(nkVulkan.logicalDevice, nkVulkan.indexBufferMemory, 0, MAX_INDEX_BUFFER, 0, &elements);
		{
			struct nk_convert_config config;
			static const struct nk_draw_vertex_layout_element vertexLayout[] =
			{
				{NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nkVertex, pos)},
				{NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nkVertex, uv)},
				{NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct nkVertex, col)},
				{NK_VERTEX_LAYOUT_END}
			};
			NK_MEMSET(&config, 0, sizeof(config));
			config.vertex_layout = vertexLayout;
			config.vertex_size = sizeof(struct nkVertex);
			config.vertex_alignment = NK_ALIGNOF(struct nkVertex);
			config.null = nkVulkan.nullTexture;
			config.circle_segment_count = 22;
			config.curve_segment_count = 22;
			config.arc_segment_count = 22;
			config.global_alpha = 1.0f;
			config.shape_AA = AA;
			config.line_AA = AA;

			nk_buffer_init_fixed(&verticesBuffer, vertices, (size_t)MAX_VERTEX_BUFFER);
			nk_buffer_init_fixed(&elementsBuffer, elements, (size_t)MAX_INDEX_BUFFER);
			nk_convert(nkContext, &nkCmd, &verticesBuffer, &elementsBuffer, &config);
		}

		vkUnmapMemory(nkVulkan.logicalDevice, nkVulkan.vertexBufferMemory);
		vkUnmapMemory(nkVulkan.logicalDevice, nkVulkan.indexBufferMemory);

		VkDeviceSize deviceOffset = 0;
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &nkVulkan.vertexBuffer, &deviceOffset);
		vkCmdBindIndexBuffer(commandBuffer, nkVulkan.indexBuffer, 0, VK_INDEX_TYPE_UINT16);

		nk_draw_foreach(cmd, nkContext, &nkCmd)
		{
			if (!cmd->elem_count) continue;

			VkRect2D scissor = { 0 };
			scissor.extent.width = cmd->clip_rect.w * fbScale.x;
			scissor.extent.height = cmd->clip_rect.h * fbScale.y;
			scissor.offset.x = max(cmd->clip_rect.x * fbScale.x, 0);
			scissor.offset.y = max(cmd->clip_rect.y * fbScale.y, 0);

			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
			vkCmdDrawIndexed(commandBuffer, cmd->elem_count, 1, offset, 0, 0);
			offset += cmd->elem_count;
		}
		nk_clear(nkContext);
		nk_buffer_clear(&nkCmd);
	}

	vkCmdEndRenderPass(commandBuffer);
	CheckVulkanResult(vkEndCommandBuffer(commandBuffer), "couldn't end nuklear command buffer");

	VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo submitInfo = { 0 };
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	submitInfo.pWaitDstStageMask = &waitStages;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &waitSemaphore;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &nkVulkan.renderCompleted;

	CheckVulkanResult(vkQueueSubmit(nkVulkan.graphicsQueue, 1, &submitInfo, NULL), "couldn't submit nuklear queue");
	CheckVulkanResult(vkQueueWaitIdle(nkVulkan.graphicsQueue), "couldn't wait nuklear queue");

	return nkVulkan.renderCompleted;
}

void CleanupNuklear()
{
	vkFreeCommandBuffers(nkVulkan.logicalDevice, nkVulkan.commandPool, 1, nkVulkan.commandBuffers);
	vkDestroySemaphore(nkVulkan.logicalDevice, nkVulkan.renderCompleted, VK_NULL_HANDLE);

	vkFreeMemory(nkVulkan.logicalDevice, nkVulkan.vertexBufferMemory, VK_NULL_HANDLE);
	vkFreeMemory(nkVulkan.logicalDevice, nkVulkan.indexBufferMemory, VK_NULL_HANDLE);
	vkFreeMemory(nkVulkan.logicalDevice, nkVulkan.uniformBuffersMemory, VK_NULL_HANDLE);
	vkFreeMemory(nkVulkan.logicalDevice, nkVulkan.textureImageMemory, VK_NULL_HANDLE);

	vkDestroyBuffer(nkVulkan.logicalDevice, nkVulkan.vertexBuffer, VK_NULL_HANDLE);
	vkDestroyBuffer(nkVulkan.logicalDevice, nkVulkan.indexBuffer, VK_NULL_HANDLE);
	vkDestroyBuffer(nkVulkan.logicalDevice, nkVulkan.uniformBuffers, VK_NULL_HANDLE);
	vkDestroyImage(nkVulkan.logicalDevice, nkVulkan.fontImage, VK_NULL_HANDLE);

	vkDestroySampler(nkVulkan.logicalDevice, nkVulkan.fontSampler, VK_NULL_HANDLE);
	vkDestroyImageView(nkVulkan.logicalDevice, nkVulkan.fontImageView, VK_NULL_HANDLE);

	vkDestroyPipelineLayout(nkVulkan.logicalDevice, nkVulkan.pipelineLayout, VK_NULL_HANDLE);
	vkDestroyRenderPass(nkVulkan.logicalDevice, nkVulkan.renderPass, VK_NULL_HANDLE);
	vkDestroyPipeline(nkVulkan.logicalDevice, nkVulkan.graphicsPipeline, VK_NULL_HANDLE);

	vkDestroyDescriptorSetLayout(nkVulkan.logicalDevice, nkVulkan.descriptorSetLayout, VK_NULL_HANDLE);
	vkDestroyDescriptorPool(nkVulkan.logicalDevice, nkVulkan.descriptorPool, VK_NULL_HANDLE);
	vkDestroyCommandPool(nkVulkan.logicalDevice, nkVulkan.commandPool, VK_NULL_HANDLE);

	nk_font_atlas_clear(atlas);
	nk_free(nkContext);
	nk_buffer_free(&nkCmd);
}


