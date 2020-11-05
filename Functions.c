#include "Header.h"

const uint32_t HEIGHT = 600;
const uint32_t WIDTH = 800;
const char* engineName = "MyGameEngine";
GLFWwindow* window;

const char* validationLayersNames[] = { "VK_LAYER_KHRONOS_validation" , NULL };
const char* deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, NULL };
const int MAX_FRAMES_IN_FLIGHT = 2;

const struct Vertex* vertices = NULL;
const uint16_t* indices;

double startTime = 0;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

void PrepareVertices()
{
	const uint16_t indicesTemp[] =
	{
		 0, 1, 2, 2, 3, 0
	};

	vertexHelper.indicesSize = sizeof(indicesTemp) / sizeof(uint16_t);
	
	indices = (uint16_t*)malloc(sizeof(uint16_t) * vertexHelper.indicesSize);
	if (indices == NULL)
	{
		printf("Couldnt allocate indices");
		exit(0);
	}
	memcpy(indices, indicesTemp, sizeof(indicesTemp));

	const struct Vertex verticesTemp[] =
	{
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};
	
	vertexHelper.verticesSize = sizeof(verticesTemp) / sizeof(struct Vertex);

	vertices = (struct Vertex*)malloc(sizeof(struct Vertex) * vertexHelper.verticesSize);
	if (vertices == NULL)
	{
		printf("Couldn't allocate vertices");
		exit(0);
	}
	memcpy(vertices, verticesTemp, sizeof(verticesTemp));
}

static VkVertexInputBindingDescription getBindingDescription() {
	VkVertexInputBindingDescription bindingDescription = { 0 };
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(struct Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	vertexHelper.vulkanBindingDescriptionSize = 1;

	return bindingDescription;
}

static VkVertexInputAttributeDescription* getAttributeDescriptions() {
	VkVertexInputAttributeDescription* attributeDescriptions = (VkVertexInputAttributeDescription*)malloc(sizeof(VkVertexInputAttributeDescription)*2);
	if (attributeDescriptions == NULL)
	{
		printf("failed to allocate attributeDescriptions");
	}

	vertexHelper.vulkanAttributeDescriptionSize = 2;
	
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(struct Vertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(struct Vertex, color);

	return attributeDescriptions;
}

void CreateVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexHelper.verticesSize;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&stagingBuffer, &stagingBufferMemory);

	void* data;
	vkMapMemory(context.logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices, (size_t)bufferSize);
	vkUnmapMemory(context.logicalDevice, stagingBufferMemory);

	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&context.vertexBuffer, &context.vertexBufferMemory);

	copyBuffer(stagingBuffer, context.vertexBuffer, bufferSize);

	vkDestroyBuffer(context.logicalDevice, stagingBuffer, NULL);
	vkFreeMemory(context.logicalDevice, stagingBufferMemory, NULL);
}

void CreateIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * vertexHelper.indicesSize;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&stagingBuffer, &stagingBufferMemory);

	void* data;
	vkMapMemory(context.logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices, (size_t)bufferSize);
	vkUnmapMemory(context.logicalDevice, stagingBufferMemory);

	CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&context.indexBuffer, &context.indexBufferMemory);

	copyBuffer(stagingBuffer, context.indexBuffer, bufferSize);

	vkDestroyBuffer(context.logicalDevice, stagingBuffer, NULL);
	vkFreeMemory(context.logicalDevice, stagingBufferMemory, NULL);
}

void CreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding = { 0 };
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo = { 0 };
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;
	CheckVulkanResult(vkCreateDescriptorSetLayout(context.logicalDevice, &layoutInfo, NULL, &context.descriptorSetLayout), "Failed to create descriptor set layout");
}

void CreateUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(struct UniformBufferObject);
	context.uniformBuffers = (VkBuffer*)malloc(sizeof(VkBuffer) * context.swapChainImagesCount);
	if (context.uniformBuffers == NULL)
	{
		printf("couldn't allocate uniform buffers");
		exit(0);
	}

	context.uniformBuffersMemory = (VkDeviceMemory*)malloc(sizeof(VkDeviceMemory) * context.swapChainImagesCount);
	if (context.uniformBuffersMemory == NULL)
	{
		printf("couldn't allocate uniform buffers memory");
		exit(0);
	}

	for (size_t i = 0; i < context.swapChainImagesCount; i++)
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &context.uniformBuffers[i], &context.uniformBuffersMemory[i]);
}

void UpdateUniformBuffer(uint32_t currentImage)
{
	
	double currentTime = glfwGetTime();
	float time = (currentTime - startTime);
	if (time > 60)
	{
		startTime = glfwGetTime();
	}

	struct UniformBufferObject ubo = { 0 };
	glm_mat4_identity(ubo.model);
	glm_rotate(ubo.model, time * glm_rad(90.0f), (vec3) { 0.0f, 0.0f, 1.0f });
	glm_lookat((vec3) { 2.0f, 2.0f, 2.0f }, (vec3) { 0.0f, 0.0f, 0.0f }, (vec3){0.0f, 0.0f, 1.0f}, ubo.view);
	glm_perspective(glm_rad(45.0f), context.swapChainExtent.width / context.swapChainExtent.height, 0.1f, 10.0f, ubo.proj);
	ubo.proj[1][1] *= -1;

	void* data;
	vkMapMemory(context.logicalDevice, context.uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(context.logicalDevice, context.uniformBuffersMemory[currentImage]);
}

void CreateDescriptorPool()
{
	VkDescriptorPoolSize poolSize = { 0 };
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = (uint32_t)context.swapChainImagesCount;

	VkDescriptorPoolCreateInfo poolInfo = { 0 };
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = (uint32_t)context.swapChainImagesCount;

	CheckVulkanResult(vkCreateDescriptorPool(context.logicalDevice, &poolInfo, NULL, &context.descriptorPool), "failed to create descriptor pool");

}

void CreateDescriptorSets()
{
	VkDescriptorSetLayout* layouts = (VkDescriptorSetLayout*)malloc(sizeof(VkDescriptorSetLayout) * context.swapChainImagesCount);
	if (layouts == NULL)
	{
		printf("couldn't allocate layouts");
		exit(0);
	}
	for (size_t i = 0; i < context.swapChainImagesCount; i++)
		layouts[i] = context.descriptorSetLayout;

	VkDescriptorSetAllocateInfo allocInfo = { 0 };
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = context.descriptorPool;
	allocInfo.descriptorSetCount = (uint32_t)context.swapChainImagesCount;
	allocInfo.pSetLayouts = layouts;

	context.descriptorSets = (VkDescriptorSet*)malloc(sizeof(VkDescriptorSet) * context.swapChainImagesCount);
	CheckVulkanResult(vkAllocateDescriptorSets(context.logicalDevice, &allocInfo, context.descriptorSets), "failed to allocate descriptor sets");

	for (size_t i = 0; i < context.swapChainImagesCount; i++)
	{
		VkDescriptorBufferInfo bufferInfo = { 0 };
		bufferInfo.buffer = context.uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(struct UniformBufferObject);

		VkWriteDescriptorSet descriptorWrite = { 0 };
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = context.descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(context.logicalDevice, 1, &descriptorWrite, 0, NULL);
	}
}

uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(context.physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	printf("failed to find suitable memory type");
	exit(0);
}

void Run()
{
	PrepareVertices();
	InitWindow();
	InitVulkan();
	MainLoop(window);
	Cleanup();
}

void InitWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, engineName, NULL, NULL);
	if (window == NULL)
	{
		printf("Couldn't create window");
		exit(0);
	}
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	context.framebufferResized = true;
}

void InitVulkan()
{
	CreateInstance();
	CreateWindowSurface();
	SetupDebugMessenger();
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateDescriptorSetLayout();
	CreateGraphicsPipeline();
	CreateFrameBuffers();
	CreateCommandPool();
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateUniformBuffers();
	CreateDescriptorPool();
	CreateDescriptorSets();
	CreateCommandBuffers();
	CreateSyncObjects();
}

void CreateInstance()
{
	if (enableValidationLayers && !CheckValidationLayerSupport())
	{
		printf("validation layers requested, but not available");
		exit(0);
	}

	VkApplicationInfo appInfo = { 0 };
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = engineName;
	appInfo.pNext = NULL;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = { 0 };
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = context.enabledLayersLength;
		createInfo.ppEnabledLayerNames = validationLayersNames;

		PopulateDebugMessengerCreateInfo(&debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	GetRequiredExtensions();
	createInfo.enabledExtensionCount = context.glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = context.glfwExtensions;

	CheckVulkanResult(vkCreateInstance(&createInfo, NULL, &context.instance), "failed to create instance");
}

void CreateWindowSurface()
{
	CheckVulkanResult(glfwCreateWindowSurface(context.instance, window, NULL, &context.surface), "Couldn't create surface");
}

void SetupDebugMessenger()
{
	if (!enableValidationLayers)
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = { 0 };
	PopulateDebugMessengerCreateInfo(&createInfo);

	CheckVulkanResult(CreateDebugUtilsMessengerEXT(context.instance, &createInfo, NULL, &context.debugMessenger), "Failed to setup debugger");
}

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo) {
	createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo->pfnUserCallback = debugCallback;
}

void PickPhysicalDevice()
{
	context.physicalDevice = VK_NULL_HANDLE;

	//check the quantity of gpus that support vulkan
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(context.instance, &deviceCount, NULL);
	if (deviceCount == 0)
	{
		printf("There is no GPU with vulkan support\n");
		exit(0);
	}

	//get said gpus.
	VkPhysicalDevice* physicalDevices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * deviceCount);
	vkEnumeratePhysicalDevices(context.instance, &deviceCount, physicalDevices);
	if (physicalDevices == NULL)
	{
		printf("couldn't get GPUS");
		exit(0);
	}


	//get the appropriate gpu that we need
	for (uint32_t i = 0; i < deviceCount; i++)
	{
		if (IsDeviceSuitable(physicalDevices[i]))
		{
			context.physicalDevice = physicalDevices[i];
			break;
		}
	}

	if (context.physicalDevice == VK_NULL_HANDLE)
	{
		printf("Failed to find a suitable GPU");
		exit(0);
	}
}

struct QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device)
{
	struct QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

	VkQueueFamilyProperties* queueFamilies = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

	int queueIndex = 0;

	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamilyFound = true;
			indices.graphicsFamily = queueIndex;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, queueIndex, context.surface, &presentSupport);
		if (presentSupport)
		{
			indices.presentFamily = queueIndex;
			indices.presentFamilyFound = true;
		}



		if (IndicesIsComplete(indices))
			break;

		queueIndex++;
	}
	return indices;
}

bool IndicesIsComplete(struct QueueFamilyIndices indices)
{
	if (indices.graphicsFamilyFound && indices.presentFamilyFound)
		return true;

	return false;
}

bool IsDeviceSuitable(VkPhysicalDevice device)
{
	struct QueueFamilyIndices indices = FindQueueFamilies(device);
	bool extensionsSupported = CheckDeviceExtensionSupport(device);
	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		QuerySwapChainSupport(device);
		if (context.swapChainDetails.formats != NULL && context.swapChainDetails.presentModes != NULL)
			swapChainAdequate = true;
	}
	return IndicesIsComplete(indices) && extensionsSupported && swapChainAdequate;
}

void CreateLogicalDevice()
{
	//getting the value of the graphicsFamily and presentFamily that we chose
	uint32_t uniqueQueueFamilies[] = { context.familyIndices.graphicsFamily, context.familyIndices.presentFamily };

	//see if the families have unique indexes, if they have, then we don't need to create more than one queueCreateInfo.
	uint32_t quantityOfUniqueQueueFamilies = 0;
	for (uint32_t i = 0; (i + 1) < sizeof(uniqueQueueFamilies) / sizeof(uint32_t); i++)
	{
		quantityOfUniqueQueueFamilies++;
		if (uniqueQueueFamilies[i] == uniqueQueueFamilies[i + 1])
		{
			break;
		}
	}

	//now we need to create the structs necessary to create a queue for both families.
	VkDeviceQueueCreateInfo* queueCreateInfos;
	queueCreateInfos = (VkDeviceQueueCreateInfo*)calloc(quantityOfUniqueQueueFamilies, sizeof(VkDeviceQueueCreateInfo));
	float queuePriority = 1.0f;
	for (uint32_t i = 0; i < quantityOfUniqueQueueFamilies; i++)
	{
		//creating an temp struct
		VkDeviceQueueCreateInfo queueCreateInfo = { 0 };
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = uniqueQueueFamilies[i];
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		//making the array of VkDeviceQueueCreateInfo point to this struct
		queueCreateInfos[i] = queueCreateInfo;
	}


	VkPhysicalDeviceFeatures deviceFeatures = { 0 };

	VkDeviceCreateInfo createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos;
	createInfo.queueCreateInfoCount = quantityOfUniqueQueueFamilies;
	createInfo.pEnabledFeatures = &deviceFeatures;
	//we decrease -1 because we use NULL in the array for delimiting when there isn't another string ahead.
	createInfo.enabledExtensionCount = (sizeof(deviceExtensions) / sizeof(char*)) - 1;
	createInfo.ppEnabledExtensionNames = deviceExtensions;
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = context.enabledLayersLength;
		createInfo.ppEnabledLayerNames = validationLayersNames;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	CheckVulkanResult(vkCreateDevice(context.physicalDevice, &createInfo, NULL, &context.logicalDevice), "failed to create logical device");

	vkGetDeviceQueue(context.logicalDevice, context.familyIndices.graphicsFamily, 0, &context.graphicsQueue);
	vkGetDeviceQueue(context.logicalDevice, context.familyIndices.presentFamily, 0, &context.presentQueue);

}

void MainLoop()
{
	//used to get FPS
	double previousTime = glfwGetTime();
	int frameCount = 0;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		DrawFrame();
		UpdateFPS(&previousTime, &frameCount);
	}

	vkDeviceWaitIdle(context.logicalDevice);
}

void Cleanup()
{
	CleanupSwapChain();

	vkDestroyDescriptorSetLayout(context.logicalDevice, context.descriptorSetLayout, NULL);

	vkDestroyBuffer(context.logicalDevice, context.vertexBuffer, NULL);
	vkFreeMemory(context.logicalDevice, context.vertexBufferMemory, NULL);

	vkDestroyBuffer(context.logicalDevice, context.indexBuffer, NULL);
	vkFreeMemory(context.logicalDevice, context.indexBufferMemory, NULL);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(context.logicalDevice, context.renderFinishedSemaphore[i], NULL);
		vkDestroySemaphore(context.logicalDevice, context.imageAvailableSemaphore[i], NULL);
		vkDestroyFence(context.logicalDevice, context.inFlightFences[i], NULL);
	}

	vkDestroyCommandPool(context.logicalDevice, context.commandPool, NULL);

	vkDestroyDevice(context.logicalDevice, NULL);
	if (enableValidationLayers)
		DestroyDebugUtilsMessengerEXT(context.instance, context.debugMessenger, NULL);

	vkDestroySurfaceKHR(context.instance, context.surface, NULL);
	vkDestroyInstance(context.instance, NULL);
	glfwDestroyWindow(window);
	glfwTerminate();

}

bool CheckValidationLayerSupport()
{
	//initializing the quantity of layers enabled
	context.enabledLayersLength = 0;

	//getting how many layers are available
	context.validationLayersLength = 0;
	CheckVulkanResult(vkEnumerateInstanceLayerProperties(&context.validationLayersLength, NULL), "Couldn't enumerate layer properties of instance");

	//getting the layers.
	context.validationLayers = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * context.validationLayersLength);
	CheckVulkanResult(vkEnumerateInstanceLayerProperties(&context.validationLayersLength, context.validationLayers), "Couldn't assign availableLayers");

	if (context.validationLayers == NULL)
	{
		printf("Couldn't allocate availableLayers");
		exit(0);
	}
	//check while array position isnt NULL
	for (uint32_t i = 0; validationLayersNames[i] != NULL; i++)
	{
		bool layerFound = false;
		for (uint32_t j = 0; j < context.validationLayersLength; j++)
		{
			//if the validation layer we want is available, increment enabledLayersSize and set layerFound to true
			if (strcmp(context.validationLayers[j].layerName, validationLayersNames[i]) == 0)
			{
				context.enabledLayersLength++;
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			return false;
		}
	}
	return true;
}

bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount = 0;
	CheckVulkanResult(vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL), "Couldn't enumerate extensions");
	VkExtensionProperties* extensionProperties;
	extensionProperties = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * extensionCount);
	CheckVulkanResult(vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, extensionProperties), "Couldn't get the device extensions");


	for (uint32_t i = 0; deviceExtensions[i] != NULL; i++)
	{
		bool foundExtension = false;
		for (uint32_t j = 0; j < extensionCount; j++)
		{
			if (strcmp(extensionProperties[j].extensionName, deviceExtensions[i]) == 0)
			{
				foundExtension = true;
				break;
			}

		}

		if (!foundExtension)
			return false;
	}

	return true;
}

void GetRequiredExtensions()
{
	//get the extensions that glfw uses.
	context.glfwExtensionCount = 0;
	const char** tempGlfwExtensions;
	tempGlfwExtensions = glfwGetRequiredInstanceExtensions(&context.glfwExtensionCount);

	//if validation layers are enabled, add it to the extensions used
	if (enableValidationLayers)
	{
		context.glfwExtensions = (const char**)malloc(sizeof(const char*) * (context.glfwExtensionCount + 1));
		for (uint32_t i = 0; i < context.glfwExtensionCount; i++)
		{
			context.glfwExtensions[i] = tempGlfwExtensions[i];
		}

		context.glfwExtensions[context.glfwExtensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
		context.glfwExtensionCount++;
	}
	else
	{
		context.glfwExtensions = tempGlfwExtensions;
	}
}

void QuerySwapChainSupport(VkPhysicalDevice device)
{
	CheckVulkanResult(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, context.surface, &context.swapChainDetails.capabilities),
		"Couldn't get physical device surface capabilities");


	CheckVulkanResult(vkGetPhysicalDeviceSurfaceFormatsKHR(device, context.surface, &context.swapChainDetails.formatsLength, NULL),
		"Couldn't enumerate physical device format capabilites");

	if (context.swapChainDetails.formatsLength != 0)
	{
		context.swapChainDetails.formats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * context.swapChainDetails.formatsLength);
		CheckVulkanResult(vkGetPhysicalDeviceSurfaceFormatsKHR(device, context.surface, &context.swapChainDetails.formatsLength, context.swapChainDetails.formats),
			"Couldn't get physical device format capabilites");
	}

	CheckVulkanResult(vkGetPhysicalDeviceSurfacePresentModesKHR(device, context.surface, &context.swapChainDetails.presentModesLength, NULL),
		"Couldn't enumerate physical device present modes capabilities");
	if (context.swapChainDetails.presentModesLength != 0)
	{
		context.swapChainDetails.presentModes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * context.swapChainDetails.presentModesLength);
		CheckVulkanResult(vkGetPhysicalDeviceSurfacePresentModesKHR(device, context.surface, &context.swapChainDetails.presentModesLength, context.swapChainDetails.presentModes),
			"Couldn't get physical device present modes capabilities");
	}

}

VkSurfaceFormatKHR ChooseSwapSurfaceFormat()
{
	for (uint32_t i = 0; i < context.swapChainDetails.formatsLength; i++)
	{
		if (context.swapChainDetails.formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && context.swapChainDetails.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return context.swapChainDetails.formats[i];
	}
	return context.swapChainDetails.formats[0];
}

VkPresentModeKHR ChoosePresentMode()
{
	for (uint32_t i = 0; i < context.swapChainDetails.presentModesLength; i++)
	{
		//dont have this mode so cant test it
		/*if (context.swapChainDetails.presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			return context.swapChainDetails.presentModes[i];*/

			//this mode doesn't limit fps
			/*if (context.swapChainDetails.presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
				return context.swapChainDetails.presentModes[i];*/
	}

	//this mode limits the fps to 60
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D ChooseSwapExtent()
{
	if (context.swapChainDetails.capabilities.currentExtent.width != UINT32_MAX)
	{
		return context.swapChainDetails.capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = { WIDTH, HEIGHT };
		actualExtent.width = maxNumber(context.swapChainDetails.capabilities.minImageExtent.width, minNumber(context.swapChainDetails.capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = maxNumber(context.swapChainDetails.capabilities.minImageExtent.height, minNumber(context.swapChainDetails.capabilities.maxImageExtent.height, actualExtent.height));
		return actualExtent;
	}
}

void CreateSwapChain()
{
	//see what type of supported swap chain the gpu has
	QuerySwapChainSupport(context.physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat();
	VkPresentModeKHR presentMode = ChoosePresentMode();
	VkExtent2D extent = ChooseSwapExtent();

	//get 1 more image than the minimum so we don't have to wait on the driver to complete it's operations to acquire another image
	uint32_t imageCount = context.swapChainDetails.capabilities.minImageCount + 1;
	if (context.swapChainDetails.capabilities.maxImageCount > 0 && imageCount > context.swapChainDetails.capabilities.maxImageCount)
		imageCount = context.swapChainDetails.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = context.surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


	//getting the value of the graphicsFamily and presentFamily that we chose
	uint32_t uniqueQueueFamilies[] = { context.familyIndices.graphicsFamily, context.familyIndices.presentFamily };
	uint32_t quantityOfUniqueQueueFamilies = 2;

	if (context.familyIndices.graphicsFamily != context.familyIndices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = uniqueQueueFamilies;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = NULL;
	}

	createInfo.preTransform = context.swapChainDetails.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	CheckVulkanResult(vkCreateSwapchainKHR(context.logicalDevice, &createInfo, NULL, &context.swapChain), "Couldn't create swap chain");

	CheckVulkanResult(vkGetSwapchainImagesKHR(context.logicalDevice, context.swapChain, &imageCount, NULL), "Couldn't enumerate swap chain images");
	context.swapChainImages = (VkImage*)malloc(sizeof(VkImage) * imageCount);
	CheckVulkanResult(vkGetSwapchainImagesKHR(context.logicalDevice, context.swapChain, &imageCount, context.swapChainImages), "Couldn't get swap chain images");
	context.swapChainImagesCount = imageCount;

	context.swapChainImageFormat = surfaceFormat.format;
	context.swapChainExtent = extent;
}

void CreateImageViews()
{
	context.swapChainImageViews = (VkImageView*)malloc(sizeof(VkImageView) * context.swapChainImagesCount);

	for (size_t i = 0; i < context.swapChainImagesCount; i++)
	{
		VkImageViewCreateInfo createInfo = { 0 };
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = context.swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = context.swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		CheckVulkanResult(vkCreateImageView(context.logicalDevice, &createInfo, NULL, &context.swapChainImageViews[i]), "Couldn't create image view");
	}
}

void CreateRenderPass()
{
	VkAttachmentDescription colorAttachment = { 0 };
	colorAttachment.format = context.swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
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
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo = { 0 };
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	CheckVulkanResult(vkCreateRenderPass(context.logicalDevice, &renderPassInfo, NULL, &context.renderPass), "failed to create render pass");
}

void CreateGraphicsPipeline()
{
	char* vertShaderCode = ReadFile("shaders/vert.spv");
	char* fragShaderCode = ReadFile("shaders/frag.spv");

	VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode, context.vertFragBufferDetails.vertexShaderBufferLength);
	VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode, context.vertFragBufferDetails.fragmentShaderBufferLength);

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
	VkVertexInputBindingDescription bindingDescription = getBindingDescription();
	vertexInputInfo.vertexBindingDescriptionCount = (uint32_t)vertexHelper.vulkanBindingDescriptionSize;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	VkVertexInputAttributeDescription* attributeDescriptions = getAttributeDescriptions();
	vertexInputInfo.vertexAttributeDescriptionCount = vertexHelper.vulkanAttributeDescriptionSize;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = { 0 };
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = { 0 };
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)context.swapChainExtent.width;
	viewport.height = (float)context.swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = { 0 };
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = context.swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState = { 0 };
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = { 0 };
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = { 0 };
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = { 0 };
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending = { 0 };
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = { 0 };
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &context.descriptorSetLayout;

	CheckVulkanResult(vkCreatePipelineLayout(context.logicalDevice, &pipelineLayoutInfo, NULL, &context.pipelineLayout), "failed to create pipeline layout");

	VkGraphicsPipelineCreateInfo pipelineInfo = { 0 };
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = context.pipelineLayout;
	pipelineInfo.renderPass = context.renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	CheckVulkanResult(vkCreateGraphicsPipelines(context.logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &context.graphicsPipeline), "failed to create graphics pipeline");
	vkDestroyShaderModule(context.logicalDevice, vertShaderModule, NULL);
	vkDestroyShaderModule(context.logicalDevice, fragShaderModule, NULL);
}

VkShaderModule CreateShaderModule(const char* bufferCode, size_t bufferCodeLength)
{
	VkShaderModuleCreateInfo createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = bufferCodeLength;
	createInfo.pCode = (uint32_t*)bufferCode;

	VkShaderModule shaderModule;
	CheckVulkanResult(vkCreateShaderModule(context.logicalDevice, &createInfo, NULL, &shaderModule), "Couldn't create shader module");
	free(bufferCode);
	return shaderModule;
}

void CreateFrameBuffers()
{
	context.swapChainFrameBuffers = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * context.swapChainImagesCount);
	if (context.swapChainFrameBuffers == NULL)
	{
		printf("Couldn't allocate memory to swap chain frame buffers");
		exit(0);
	}
	for (size_t i = 0; i < context.swapChainImagesCount; i++)
	{
		VkImageView attachments[] = { context.swapChainImageViews[i] };
		VkFramebufferCreateInfo framebufferInfo = { 0 };
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = context.renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = context.swapChainExtent.width;
		framebufferInfo.height = context.swapChainExtent.height;
		framebufferInfo.layers = 1;

		CheckVulkanResult(vkCreateFramebuffer(context.logicalDevice, &framebufferInfo, NULL, &context.swapChainFrameBuffers[i]), "failed to create framebuffer");

	}
}

void CreateCommandPool()
{
	VkCommandPoolCreateInfo poolInfo = { 0 };
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = context.familyIndices.graphicsFamily;
	poolInfo.flags = 0;

	CheckVulkanResult(vkCreateCommandPool(context.logicalDevice, &poolInfo, NULL, &context.commandPool), "failed to create command pool");
}

void CreateCommandBuffers()
{
	context.commandBuffers = (VkCommandBuffer*)malloc(sizeof(VkCommandBuffer) * context.swapChainImagesCount);
	VkCommandBufferAllocateInfo allocateInfo = { 0 };
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.commandPool = context.commandPool;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandBufferCount = context.swapChainImagesCount;

	CheckVulkanResult(vkAllocateCommandBuffers(context.logicalDevice, &allocateInfo, context.commandBuffers), "failed to allocate command buffers");

	for (size_t i = 0; i < context.swapChainImagesCount; i++)
	{
		VkCommandBufferBeginInfo beginInfo = { 0 };
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		CheckVulkanResult(vkBeginCommandBuffer(context.commandBuffers[i], &beginInfo), "failed to begin recording command buffer");

		VkRenderPassBeginInfo renderPassInfo = { 0 };
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = context.renderPass;
		renderPassInfo.framebuffer = context.swapChainFrameBuffers[i];
		renderPassInfo.renderArea.offset.x = 0;
		renderPassInfo.renderArea.offset.y = 0;
		renderPassInfo.renderArea.extent = context.swapChainExtent;
		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(context.commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(context.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, context.graphicsPipeline);
		
		VkBuffer vertexBuffers[] = { context.vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(context.commandBuffers[i], 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(context.commandBuffers[i], context.indexBuffer, 0, VK_INDEX_TYPE_UINT16);

		vkCmdBindDescriptorSets(context.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, context.pipelineLayout, 0, 1, &context.descriptorSets[i], 0, NULL);

		vkCmdDrawIndexed(context.commandBuffers[i], (uint32_t)(vertexHelper.indicesSize), 1, 0, 0, 0);
		vkCmdEndRenderPass(context.commandBuffers[i]);

		CheckVulkanResult(vkEndCommandBuffer(context.commandBuffers[i]), "failed to record command buffer");
	}
}

void CreateSyncObjects()
{
	context.imageAvailableSemaphore = (VkSemaphore*)malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
	context.renderFinishedSemaphore = (VkSemaphore*)malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
	context.inFlightFences = (VkFence*)malloc(sizeof(VkFence) * MAX_FRAMES_IN_FLIGHT);
	context.imagesInFlight = (VkFence*)malloc(sizeof(VkFence) * context.swapChainImagesCount);

	if (context.imageAvailableSemaphore == NULL || context.renderFinishedSemaphore == NULL || context.inFlightFences == NULL)
	{
		printf("failed to allocate memory to semaphores");
		exit(0);
	}

	VkSemaphoreCreateInfo semaphoreInfo = { 0 };
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = { 0 };
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	//initializing fences and semaphores
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		CheckVulkanResult(vkCreateSemaphore(context.logicalDevice, &semaphoreInfo, NULL, &context.imageAvailableSemaphore[i]), "failed to create synchronization objects for a frame");
		CheckVulkanResult(vkCreateSemaphore(context.logicalDevice, &semaphoreInfo, NULL, &context.renderFinishedSemaphore[i]), "failed to create synchronization objects for a frame");
		CheckVulkanResult(vkCreateFence(context.logicalDevice, &fenceInfo, NULL, &context.inFlightFences[i]), "failed to create synchronization objects for a frame");
		context.imagesInFlight[i] = VK_NULL_HANDLE;
	}
}

void DrawFrame()
{
	vkWaitForFences(context.logicalDevice, 1, &context.inFlightFences[context.currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(context.logicalDevice, context.swapChain, UINT64_MAX, 
		 context.imageAvailableSemaphore[context.currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapchain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		printf("Failed to acquire swap chain image");
		exit(0);
	}

	if (context.imagesInFlight[imageIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(context.logicalDevice, 1, &context.imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}

	context.imagesInFlight[imageIndex] = context.inFlightFences[context.currentFrame];

	UpdateUniformBuffer(imageIndex);

	VkSubmitInfo submitInfo = { 0 };
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { context.imageAvailableSemaphore[context.currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &context.commandBuffers[imageIndex];
	VkSemaphore signalSemaphores[] = { context.renderFinishedSemaphore[context.currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(context.logicalDevice, 1, &context.inFlightFences[context.currentFrame]);

	CheckVulkanResult(vkQueueSubmit(context.graphicsQueue, 1, &submitInfo, context.inFlightFences[context.currentFrame]), "failed to submit draw command buffer");

	VkPresentInfoKHR presentInfo = { 0 };
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { context.swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(context.presentQueue, &presentInfo);
	
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || context.framebufferResized)
	{
		context.framebufferResized = false;
		RecreateSwapchain();
	}
	else if (result != VK_SUCCESS)
	{
		printf("failed to present swap chain image");
		exit(0);
	}

	context.currentFrame = (context.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void CleanupSwapChain()
{
	for (size_t i = 0; i < context.swapChainImagesCount; i++)
		vkDestroyFramebuffer(context.logicalDevice, context.swapChainFrameBuffers[i], NULL);

	vkFreeCommandBuffers(context.logicalDevice, context.commandPool, context.swapChainImagesCount, context.commandBuffers);

	vkDestroyPipeline(context.logicalDevice, context.graphicsPipeline, NULL);
	vkDestroyPipelineLayout(context.logicalDevice, context.pipelineLayout, NULL);
	vkDestroyRenderPass(context.logicalDevice, context.renderPass, NULL);

	for (size_t i = 0; i < context.swapChainImagesCount; i++)
		vkDestroyImageView(context.logicalDevice, context.swapChainImageViews[i], NULL);

	vkDestroySwapchainKHR(context.logicalDevice, context.swapChain, NULL);

	for (size_t i = 0; i < context.swapChainImagesCount; i++)
	{
		vkDestroyBuffer(context.logicalDevice, context.uniformBuffers[i], NULL);
		vkFreeMemory(context.logicalDevice, context.uniformBuffersMemory[i], NULL);
	}

	vkDestroyDescriptorPool(context.logicalDevice, context.descriptorPool, NULL);
}

void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory)
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
	/*
	It should be noted that in a real world application, you're not supposed to actually call vkAllocateMemory for every individual buffer. The maximum number of 
	simultaneous memory allocations is limited by the maxMemoryAllocationCount physical device limit, which may be as low as 4096 even on high end hardware like 
	an NVIDIA GTX 1080. The right way to allocate memory for a large number of objects at the same time is to create a custom allocator that splits up a single 
	allocation among many different objects by using the offset parameters that we've seen in many functions.

	You can either implement such an allocator yourself, or use the VulkanMemoryAllocator library provided by the GPUOpen initiative. However, for this tutorial 
	it's okay to use a separate allocation for every resource, because we won't come close to hitting any of these limits for now.
	*/
	CheckVulkanResult(vkAllocateMemory(context.logicalDevice, &allocInfo, NULL, &*bufferMemory), "failed to allocate vertex buffer memory");

	vkBindBufferMemory(context.logicalDevice, *buffer, *bufferMemory, 0);
}

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBufferAllocateInfo allocInfo = { 0 };
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = context.commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(context.logicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = { 0 };
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion = { 0 };
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = { 0 };
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(context.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	//later implement fences here, because it allows the programmer to schedule multiple transfer simultaneously instead of one at a time.
	vkQueueWaitIdle(context.graphicsQueue);

	vkFreeCommandBuffers(context.logicalDevice, context.commandPool, 1, &commandBuffer);
}

void RecreateSwapchain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(context.logicalDevice);

	CleanupSwapChain();

	CreateSwapChain();
	CreateImageViews();
	CreateRenderPass();
	CreateGraphicsPipeline();
	CreateFrameBuffers();
	CreateUniformBuffers();
	CreateDescriptorPool();
	CreateDescriptorSets();
	CreateCommandBuffers();
}

static char* ReadFile(const char* fileName)
{
	FILE* shaderStream;
	if (fopen_s(&shaderStream, fileName, "rb") != NULL)
	{
		printf("failed to open file");
		exit(0);
	}

	fseek(shaderStream, 0, SEEK_END);
	size_t length = ftell(shaderStream);
	fseek(shaderStream, 0, SEEK_SET);
	char* buffer = (char*)malloc(length);
	fread(buffer, sizeof(char), length, shaderStream);
	fclose(shaderStream);

	//saving length to reference it later
	if (!context.vertFragBufferDetails.vertexShaderBufferFound)
	{
		context.vertFragBufferDetails.vertexShaderBufferLength = length;
		context.vertFragBufferDetails.vertexShaderBufferFound = true;
	}
	else
	{
		context.vertFragBufferDetails.fragmentShaderBufferLength = length;
	}

	return buffer;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	printf("validation layer: %s\n", pCallbackData->pMessage);
	return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != NULL) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void CheckVulkanResult(VkResult result, char* string)
{
	if (result != VK_SUCCESS)
	{
		printf("%s\n", string);
		exit(0);
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != NULL) {
		func(instance, debugMessenger, pAllocator);
	}
}

void* maxNumber(void* a, void* b)
{
	return a > b ? a : b;
}

void* minNumber(void* a, void* b)
{
	return a > b ? b : a;
}

void UpdateFPS(double* previousTime, int* frameCount)
{
	// Measure speed
	double currentTime = glfwGetTime();
	*frameCount += 1;
	// If a second has passed.5
	if (currentTime - *previousTime >= 1.0)
	{
		char title[255];
		snprintf(title, 255, "%s - [FPS: %d]", engineName, *frameCount);
		// Display the frame count here any way you want.
		glfwSetWindowTitle(window, &title);

		*frameCount = 0;
		*previousTime = currentTime;
	}
}