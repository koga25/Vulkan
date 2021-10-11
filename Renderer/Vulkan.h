#pragma once

#define FRAME_OVERLAP 2
#include "VulkanInitializers.h"
#include "vk_mem_alloc.h"
#include "SDL_vulkan.h"
#include "SDL.h"
#include "../Utils/Enums.h"


#include <ft2build.h>
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION
#include "../Vendors/stb_image.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cglm/cglm.h>
#include <math.h>


struct UploadedContext
{
	VkFence uploadFence;
	VkCommandPool commandPool;
};

struct AllocatedImage
{
	VkImage image;
	VmaAllocation allocation;
};

struct Texture
{
	struct AllocatedImage image;
	VkImageView imageView;
};

struct MeshPushConstants
{
	vec2 screenDimensions;
	vec2 tileOffset;
	float sizeOfTile;
	uint32_t characterLayer;
	float guiAnimationTime;
};

struct AllocatedBuffer
{
	VkBuffer buffer;
	VmaAllocation allocation;
};

struct Vertex
{
	vec3 position;
	vec3 normal;
	vec2 textureUV;
};

struct Mesh
{
	struct Vertex* vertices;
	uint32_t vertexSize;
	struct AllocatedBuffer vertexBuffer;
};

struct FrameData
{
	VkSemaphore presentSemaphore;
	VkSemaphore renderSemaphore;
	VkFence renderFence;

	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
	VkDescriptorSet globalDescriptor;
};

struct TextureArrayDimensions
{
	uint16_t texturesPerCol;
	uint16_t texturesPerRow;
	uint32_t RGBA_PerHorizontalLine;
	uint32_t pixelsPerVerticalLine;
};

struct VulkanHelper
{
	bool isInitialized;
	int frameNumber;
	VkExtent2D windowExtent;
	SDL_Window* mainWindow;

	VkInstance instance;


	VkLayerProperties* validationLayers;
	uint32_t validationLayersLength;
	uint32_t enabledLayersLength;
	VkDebugUtilsMessengerEXT debugMessenger;

	uint32_t sdlExtensionCount;
	const char** sdlExtensions;

	VkSurfaceKHR surface;

	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties physicalDeviceProperties;
	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

	struct QueueFamilyIndices
	{
		uint32_t graphicsFamily;
		bool graphicsFamilyFound;

		//queue family that has the capability of presenting to window surface.
		uint32_t presentFamily;
		bool presentFamilyFound;
	}familyIndices;

	VkDevice logicalDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		VkSurfaceFormatKHR* formats;
		uint32_t formatsLength;
		VkPresentModeKHR* presentModes;
		uint32_t presentModesLength;
	}swapChainDetails;
	VkSwapchainKHR swapChain;
	VkImage* swapChainImages;
	uint32_t swapChainImagesCount;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	VkImageView* swapChainImageViews;

	VkRenderPass renderPass;
	VkFramebuffer* framebuffers;

	struct FrameData frames[FRAME_OVERLAP];
	struct FrameData currentFrame;
	uint32_t currentSwapChainImageIndex;

	VmaAllocator allocator;

	VkPipeline meshPipeline;
	VkPipelineLayout meshPipelineLayout;
	VkPipeline guiPipeline;

	struct Mesh spriteMesh;
	struct Mesh characterMesh;
	struct Mesh guiMesh;

	VkDescriptorSetLayout globalSetLayout;
	VkDescriptorPool descriptorPool;

	struct UploadedContext uploadContext;

	struct Texture* loadedTextures;
	uint32_t loadedTexturesSize;

	VkDescriptorSetLayout textureSetLayout;
	VkDescriptorSet textureSet;

	VkDescriptorSetLayout characterSetLayout;
	VkDescriptorSet characterSet;
	uint8_t characterSize;
}vulkanContext;

struct FreetypeHelper
{
	FT_Library library;
	FT_Face face;
	FT_GlyphSlot glyph;
}freeTypeHelper;

struct PipelineBuilder
{
	VkPipelineShaderStageCreateInfo* shaderStages;
	uint32_t shaderStagesLength;

	VkPipelineVertexInputStateCreateInfo vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssembly;
	VkViewport viewport;
	VkRect2D scissor;
	VkPipelineRasterizationStateCreateInfo rasterizer;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineMultisampleStateCreateInfo multisampling;
	VkPipelineLayout pipelineLayout;
	//VkPipelineDepthStencilStateCreateInfo depthStencil;
};

struct VertexInputDescription
{
	VkVertexInputBindingDescription* bindings;
	uint32_t bindingsSize;

	VkVertexInputAttributeDescription* attributes;
	uint32_t attributesSize;

	VkPipelineVertexInputStateCreateFlags flags;
};



const char* validationLayersNames[] = { "VK_LAYER_KHRONOS_validation" , NULL };
const char* deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, NULL };
const int MAX_FRAMES_IN_FLIGHT = 2;

SDL_Window* mainWindow;
SDL_Renderer* mainRenderer = NULL;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

void InitVulkan();
void InitFreeType(struct AllocatedImage* outImage);
void CreateInstance();
bool CheckValidationLayerSupport();
void GetRequiredExtensions();
void SetupDebugMessenger();
void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo);
void CreateSurface();
void PickPhysicalDevice();
bool IsDeviceSuitable(VkPhysicalDevice device);
struct QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
bool IndicesIsComplete(struct QueueFamilyIndices indices);
bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
void QuerySwapChainSupport(VkPhysicalDevice device);
void CreateLogicalDevice();
void InitVmaAllocator();
VkSurfaceFormatKHR ChooseSwapSurfaceFormat();
VkPresentModeKHR ChoosePresentMode();
VkExtent2D ChooseSwapExtent();
void CreateSwapChain();
void CreateImageViews();
VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
void InitCommands();
void InitDefaultRenderpass();
void InitFramebuffers();
void InitSyncStructures();
bool LoadShaderModule(const char* filePath, VkShaderModule* outShaderModule);
void InitPipelines();
VkPipeline BuildPipeline(VkDevice device, VkRenderPass renderPass, struct PipelineBuilder pipelineBuilder);
void InitMeshes();
void CreateMesh(uint16_t sizeOfQuad, struct Mesh* mesh);
void UploadMesh(struct Mesh* mesh);
void ChangeVertexBuffer();
static struct VertexInputDescription GetVertexDescription();
struct FrameData* GetCurrentFrame();
uint32_t InitiateDrawCalls();
void BindSpriteSheet(VkCommandBuffer commandBuffer);
void BindCharacterSheet(VkCommandBuffer commandBuffer);
void BindGuiPipeline(VkCommandBuffer commandBuffer);
void DrawMap(VkCommandBuffer commandBuffer, struct MapHelper* map);
void DrawString(VkCommandBuffer commandBuffer, char* string, uint16_t xPos, uint16_t yPos);
void SubmitDrawCalls(uint32_t swapchainImageIndex);
void InitDescriptorSets();
struct AllocatedBuffer CreateBuffer(size_t AllocationSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
void InitDescriptors();
void BufferImmediateSubmit(const size_t bufferSize, struct AllocatedBuffer* stagingBuffer, struct Mesh* mesh, bool uploadingTexture, struct AllocatedImage* newImage, VkExtent3D* imageExtent);
void ImageImmediateSubmit(struct AllocatedBuffer* stagingBuffer, struct AllocatedImage* newImage, VkExtent3D* imageExtent, uint32_t layerOffset, uint32_t arrayLayers);
void loadImages();
void CleanupVulkan();
void DeletionQueue();
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData);
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
void CheckVulkanResult(VkResult result, char* string);
void* maxNumber(void* a, void* b);
void* minNumber(void* a, void* b);

////////////////////////////////////TEXTURE LOADING//////////////////////////////////////
//this functions reads a texture atlas and transforms the memory layout to one that can be used by texture arrays.
bool loadImageFromFile(const char* file, struct AllocatedImage* outImage, uint32_t arrayLayers, struct TextureArrayDimensions textArrDimensions, VkExtent3D imageExtent);

void InitVulkan()
{
	SDL_WindowFlags windowFlags = (SDL_WindowFlags)SDL_WINDOW_VULKAN;
	//Screen dimensions
	vulkanContext.windowExtent.width = 1280;
	vulkanContext.windowExtent.height = 720;

	vulkanContext.mainWindow = SDL_CreateWindow(
		"MyRogueLike",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		vulkanContext.windowExtent.width,
		vulkanContext.windowExtent.height,
		windowFlags
	);

	vulkanContext.frameNumber = 0;
	
	CreateInstance();
	SetupDebugMessenger();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
	InitVmaAllocator();
	CreateSwapChain();
	CreateImageViews();
	InitDefaultRenderpass();
	InitFramebuffers();
	InitCommands();
	InitSyncStructures();
	InitDescriptors();
	InitPipelines();
	InitMeshes();
	loadImages();
	InitDescriptorSets();

	vulkanContext.isInitialized = true;

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
	appInfo.pApplicationName = "MyRogueLike";
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
		createInfo.enabledLayerCount = vulkanContext.enabledLayersLength;
		createInfo.ppEnabledLayerNames = validationLayersNames;

		PopulateDebugMessengerCreateInfo(&debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	GetRequiredExtensions();
	createInfo.enabledExtensionCount = vulkanContext.sdlExtensionCount;
	createInfo.ppEnabledExtensionNames = vulkanContext.sdlExtensions;

	CheckVulkanResult(vkCreateInstance(&createInfo, NULL, &vulkanContext.instance), "failed to create instance");
}

void GetRequiredExtensions()
{
	//get the extensions that glfw uses.
	vulkanContext.sdlExtensionCount = 0;
	const char** tempSdlExtensions = NULL;
	if (!SDL_Vulkan_GetInstanceExtensions(vulkanContext.mainWindow, &vulkanContext.sdlExtensionCount, NULL))
	{
		printf("couldn't get SDL vulkan extensions count\n");
		exit(1);
	}

	tempSdlExtensions = (const char**)malloc(sizeof(const char*) * (vulkanContext.sdlExtensionCount + 1));

	if (!SDL_Vulkan_GetInstanceExtensions(vulkanContext.mainWindow, &vulkanContext.sdlExtensionCount, tempSdlExtensions))
	{
		printf("couldn't get SDL vulkan extensions names\n");
		exit(1);
	}


	//if validation layers are enabled, add it to the extensions used
	if (enableValidationLayers)
	{
		vulkanContext.sdlExtensions = (const char**)malloc(sizeof(const char*) * (vulkanContext.sdlExtensionCount + 1));
		for (uint32_t i = 0; i < vulkanContext.sdlExtensionCount; i++)
		{
			vulkanContext.sdlExtensions[i] = tempSdlExtensions[i];
		}

		vulkanContext.sdlExtensions[vulkanContext.sdlExtensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
		vulkanContext.sdlExtensionCount++;
	}
	else
	{
		vulkanContext.sdlExtensions = tempSdlExtensions;
	}
}

bool CheckValidationLayerSupport()
{
	//initializing the quantity of layers enabled
	vulkanContext.enabledLayersLength = 0;

	//getting how many layers are available
	vulkanContext.validationLayersLength = 0;
	CheckVulkanResult(vkEnumerateInstanceLayerProperties(&vulkanContext.validationLayersLength, NULL), "Couldn't enumerate layer properties of instance");

	//getting the layers.
	vulkanContext.validationLayers = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * vulkanContext.validationLayersLength);
	CheckVulkanResult(vkEnumerateInstanceLayerProperties(&vulkanContext.validationLayersLength, vulkanContext.validationLayers), "Couldn't assign availableLayers");

	if (vulkanContext.validationLayers == NULL)
	{
		printf("Couldn't allocate availableLayers");
		exit(0);
	}
	//check while array position isnt NULL
	for (uint32_t i = 0; validationLayersNames[i] != NULL; i++)
	{
		bool layerFound = false;
		for (uint32_t j = 0; j < vulkanContext.validationLayersLength; j++)
		{
			//if the validation layer we want is available, increment enabledLayersSize and set layerFound to true
			if (strcmp(vulkanContext.validationLayers[j].layerName, validationLayersNames[i]) == 0)
			{
				vulkanContext.enabledLayersLength++;
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

void SetupDebugMessenger()
{
	if (!enableValidationLayers)
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = { 0 };
	PopulateDebugMessengerCreateInfo(&createInfo);

	CheckVulkanResult(CreateDebugUtilsMessengerEXT(vulkanContext.instance, &createInfo, NULL, &vulkanContext.debugMessenger), "Failed to setup debugger");
}

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo)
{
	createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo->pfnUserCallback = debugCallback;
}

void CreateSurface()
{
	SDL_Vulkan_CreateSurface(vulkanContext.mainWindow, vulkanContext.instance, &vulkanContext.surface);
}

void PickPhysicalDevice()
{
	vulkanContext.physicalDevice = VK_NULL_HANDLE;

	//check the quantity of gpus that support vulkan
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vulkanContext.instance, &deviceCount, NULL);
	if (deviceCount == 0)
	{
		printf("There is no GPU with vulkan support\n");
		exit(0);
	}

	//get said gpus.
	VkPhysicalDevice* physicalDevices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * deviceCount);
	vkEnumeratePhysicalDevices(vulkanContext.instance, &deviceCount, physicalDevices);
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
			vulkanContext.physicalDevice = physicalDevices[i];
			break;
		}
	}

	if (vulkanContext.physicalDevice == VK_NULL_HANDLE)
	{
		printf("Failed to find a suitable GPU");
		exit(0);
	}
}

bool IsDeviceSuitable(VkPhysicalDevice device)
{
	struct QueueFamilyIndices indices = FindQueueFamilies(device);
	bool extensionsSupported = CheckDeviceExtensionSupport(device);
	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		QuerySwapChainSupport(device);
		if (vulkanContext.swapChainDetails.formats != NULL && vulkanContext.swapChainDetails.presentModes != NULL)
			swapChainAdequate = true;
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return IndicesIsComplete(indices) && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

struct QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device)
{
	struct QueueFamilyIndices indices = { 0 };
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
		vkGetPhysicalDeviceSurfaceSupportKHR(device, queueIndex, vulkanContext.surface, &presentSupport);
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

void QuerySwapChainSupport(VkPhysicalDevice device)
{
	CheckVulkanResult(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vulkanContext.surface, &vulkanContext.swapChainDetails.capabilities),
		"Couldn't get physical device surface capabilities");


	CheckVulkanResult(vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkanContext.surface, &vulkanContext.swapChainDetails.formatsLength, NULL),
		"Couldn't enumerate physical device format capabilites");

	if (vulkanContext.swapChainDetails.formatsLength != 0)
	{
		vulkanContext.swapChainDetails.formats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * vulkanContext.swapChainDetails.formatsLength);
		CheckVulkanResult(vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkanContext.surface, &vulkanContext.swapChainDetails.formatsLength, vulkanContext.swapChainDetails.formats),
			"Couldn't get physical device format capabilites");
	}

	CheckVulkanResult(vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkanContext.surface, &vulkanContext.swapChainDetails.presentModesLength, NULL),
		"Couldn't enumerate physical device present modes capabilities");
	if (vulkanContext.swapChainDetails.presentModesLength != 0)
	{
		vulkanContext.swapChainDetails.presentModes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * vulkanContext.swapChainDetails.presentModesLength);
		CheckVulkanResult(vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkanContext.surface, &vulkanContext.swapChainDetails.presentModesLength, vulkanContext.swapChainDetails.presentModes),
			"Couldn't get physical device present modes capabilities");
	}

}

void CreateLogicalDevice()
{
	//getting the value of the graphicsFamily and presentFamily that we chose
	uint32_t uniqueQueueFamilies[] = { vulkanContext.familyIndices.graphicsFamily, vulkanContext.familyIndices.presentFamily };

	//see if the families have unique indexes, if they have, then we don't need to create more than one queueCreateInfo.
	uint32_t quantityOfUniqueQueueFamilies = 0;
	for (uint32_t i = 0; i < sizeof(uniqueQueueFamilies) / sizeof(uint32_t); i++)
	{
		quantityOfUniqueQueueFamilies++;
		if (uniqueQueueFamilies[i] == uniqueQueueFamilies[(i + 1)])
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
		memcpy(&queueCreateInfos[i], &queueCreateInfo, (size_t)sizeof(VkDeviceQueueCreateInfo));
		//queueCreateInfos[i] = queueCreateInfo;
	}


	VkPhysicalDeviceFeatures deviceFeatures = { 0 };
	deviceFeatures.samplerAnisotropy = VK_TRUE;

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
		createInfo.enabledLayerCount = vulkanContext.enabledLayersLength;
		createInfo.ppEnabledLayerNames = validationLayersNames;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	CheckVulkanResult(vkCreateDevice(vulkanContext.physicalDevice, &createInfo, NULL, &vulkanContext.logicalDevice), "failed to create logical device");

	vkGetDeviceQueue(vulkanContext.logicalDevice, vulkanContext.familyIndices.graphicsFamily, 0, &vulkanContext.graphicsQueue);
	vkGetDeviceQueue(vulkanContext.logicalDevice, vulkanContext.familyIndices.presentFamily, 0, &vulkanContext.presentQueue);

}

VkSurfaceFormatKHR ChooseSwapSurfaceFormat()
{
	for (uint32_t i = 0; i < vulkanContext.swapChainDetails.formatsLength; i++)
	{
		if (vulkanContext.swapChainDetails.formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && vulkanContext.swapChainDetails.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return vulkanContext.swapChainDetails.formats[i];
	}
	return vulkanContext.swapChainDetails.formats[0];
}

VkPresentModeKHR ChoosePresentMode()
{
	for (uint32_t i = 0; i < vulkanContext.swapChainDetails.presentModesLength; i++)
	{
		//dont have this mode so cant test it
		/*if (context.swapChainDetails.presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			return context.swapChainDetails.presentModes[i];*/

			//this mode doesn't limit fps
			/*if (vulkanContext.swapChainDetails.presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
				return vulkanContext.swapChainDetails.presentModes[i];*/
	}

	//this mode limits the fps to 60
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D ChooseSwapExtent()
{
	if (vulkanContext.swapChainDetails.capabilities.currentExtent.width != UINT32_MAX)
	{
		return vulkanContext.swapChainDetails.capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = { 1280, 720};
		actualExtent.width = (uint32_t)maxNumber(vulkanContext.swapChainDetails.capabilities.minImageExtent.width, minNumber(vulkanContext.swapChainDetails.capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = (uint32_t)maxNumber(vulkanContext.swapChainDetails.capabilities.minImageExtent.height, minNumber(vulkanContext.swapChainDetails.capabilities.maxImageExtent.height, actualExtent.height));
		return actualExtent;
	}
}

void InitVmaAllocator()
{
	VmaAllocatorCreateInfo allocatorInfo = { 0 };
	allocatorInfo.physicalDevice = vulkanContext.physicalDevice;
	allocatorInfo.device = vulkanContext.logicalDevice;
	allocatorInfo.instance = vulkanContext.instance;

	vmaCreateAllocator(&allocatorInfo, &vulkanContext.allocator);
}

void CreateSwapChain()
{
	//see what type of supported swap chain the gpu has
	QuerySwapChainSupport(vulkanContext.physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat();
	VkPresentModeKHR presentMode = ChoosePresentMode();
	VkExtent2D extent = ChooseSwapExtent();

	//get 1 more image than the minimum so we don't have to wait on the driver to complete it's operations to acquire another image
	uint32_t imageCount = vulkanContext.swapChainDetails.capabilities.minImageCount + 1;
	if (vulkanContext.swapChainDetails.capabilities.maxImageCount > 0 && imageCount > vulkanContext.swapChainDetails.capabilities.maxImageCount)
		imageCount = vulkanContext.swapChainDetails.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = vulkanContext.surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


	//getting the value of the graphicsFamily and presentFamily that we chose
	uint32_t uniqueQueueFamilies[] = { vulkanContext.familyIndices.graphicsFamily, vulkanContext.familyIndices.presentFamily };
	uint32_t quantityOfUniqueQueueFamilies = 2;

	if (vulkanContext.familyIndices.graphicsFamily != vulkanContext.familyIndices.presentFamily)
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

	createInfo.preTransform = vulkanContext.swapChainDetails.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	CheckVulkanResult(vkCreateSwapchainKHR(vulkanContext.logicalDevice, &createInfo, NULL, &vulkanContext.swapChain), "Couldn't create swap chain");

	CheckVulkanResult(vkGetSwapchainImagesKHR(vulkanContext.logicalDevice, vulkanContext.swapChain, &imageCount, NULL), "Couldn't enumerate swap chain images");
	vulkanContext.swapChainImages = (VkImage*)malloc(sizeof(VkImage) * imageCount);
	CheckVulkanResult(vkGetSwapchainImagesKHR(vulkanContext.logicalDevice, vulkanContext.swapChain, &imageCount, vulkanContext.swapChainImages), "Couldn't get swap chain images");
	vulkanContext.swapChainImagesCount = imageCount;

	vulkanContext.swapChainImageFormat = surfaceFormat.format;
	vulkanContext.swapChainExtent = extent;


}

void CreateImageViews()
{
	vulkanContext.swapChainImageViews = (VkImageView*)malloc(sizeof(VkImageView) * vulkanContext.swapChainImagesCount);

	for (size_t i = 0; i < vulkanContext.swapChainImagesCount; i++)
	{
		vulkanContext.swapChainImageViews[i] = CreateImageView(vulkanContext.swapChainImages[i], vulkanContext.swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewInfo = { 0 };
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	CheckVulkanResult(vkCreateImageView(vulkanContext.logicalDevice, &viewInfo, NULL, &imageView), "failed to create texture image view");

	return imageView;
}

void InitCommands()
{
	VkCommandPoolCreateInfo commandPoolInfo = CreateCommandPoolCreateInfo(vulkanContext.familyIndices.graphicsFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	for (int i = 0; i < FRAME_OVERLAP; i++)
	{
		CheckVulkanResult(vkCreateCommandPool(vulkanContext.logicalDevice, &commandPoolInfo, NULL, &vulkanContext.frames[i].commandPool), "Couldn't create command pool");

		VkCommandBufferAllocateInfo commandBufferAllocateInfo = CreateCommandBufferAllocateInfo(vulkanContext.frames[i].commandPool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		CheckVulkanResult(vkAllocateCommandBuffers(vulkanContext.logicalDevice, &commandBufferAllocateInfo, &vulkanContext.frames[i].commandBuffer), "Couldn't create command buffer");
	}

	VkCommandPoolCreateInfo uploadCommandInfo = CreateCommandPoolCreateInfo(vulkanContext.familyIndices.graphicsFamily, 0);
	CheckVulkanResult(vkCreateCommandPool(vulkanContext.logicalDevice, &uploadCommandInfo, NULL, &vulkanContext.uploadContext.commandPool), "couldn't create upload coomand pool\n");
}

void InitDefaultRenderpass()
{
	VkAttachmentDescription colorAttachment = { 0 };
	colorAttachment.format = vulkanContext.swapChainImageFormat;
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

	VkAttachmentDescription attachments[] = { colorAttachment };

	VkRenderPassCreateInfo renderPassInfo = { 0 };
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &attachments[0];
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	CheckVulkanResult(vkCreateRenderPass(vulkanContext.logicalDevice, &renderPassInfo, NULL, &vulkanContext.renderPass), "couldn't create render pass");
}

void InitFramebuffers()
{
	VkFramebufferCreateInfo framebufferInfo = { 0 };
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.pNext = NULL;
	framebufferInfo.renderPass = vulkanContext.renderPass;
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.width = vulkanContext.windowExtent.width;
	framebufferInfo.height = vulkanContext.windowExtent.height;
	framebufferInfo.layers = 1;

	vulkanContext.framebuffers = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * vulkanContext.swapChainImagesCount);

	for (uint32_t i = 0; i < vulkanContext.swapChainImagesCount; i++)
	{
		VkImageView attachments[1];
		attachments[0] = vulkanContext.swapChainImageViews[i];

		framebufferInfo.pAttachments = attachments;
		framebufferInfo.attachmentCount = 1;
		CheckVulkanResult(vkCreateFramebuffer(vulkanContext.logicalDevice, &framebufferInfo, NULL, &vulkanContext.framebuffers[i]), "Couldn't create frame buffer ");
	}
}

void InitSyncStructures()
{
	
	VkFenceCreateInfo fenceCreateInfo = CreateFenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);

	VkSemaphoreCreateInfo semaphoreCreateInfo = CreateSemaphoreCreateInfo(0);

	for (int i = 0; i < FRAME_OVERLAP; i++)
	{
		CheckVulkanResult(vkCreateFence(vulkanContext.logicalDevice, &fenceCreateInfo, NULL, &vulkanContext.frames[i].renderFence), "Couldn't create fence");

		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = NULL;
		semaphoreCreateInfo.flags = 0;

		CheckVulkanResult(vkCreateSemaphore(vulkanContext.logicalDevice, &semaphoreCreateInfo, NULL, &vulkanContext.frames[i].presentSemaphore), "Couldn't create present semaphore");
		CheckVulkanResult(vkCreateSemaphore(vulkanContext.logicalDevice, &semaphoreCreateInfo, NULL, &vulkanContext.frames[i].renderSemaphore), "Couldn't create render semaphore");
	}

	VkFenceCreateInfo uploadFenceCreateInfo = CreateFenceCreateInfo(0);
	CheckVulkanResult(vkCreateFence(vulkanContext.logicalDevice, &uploadFenceCreateInfo, NULL, &vulkanContext.uploadContext.uploadFence), "couldn't create upload fence\n");
}

bool LoadShaderModule(const char* filePath, VkShaderModule* outShaderModule)
{
	FILE* file;
	fopen_s(&file, filePath, "rb");
	if (file == NULL)
	{
		return false;
	}

	fseek(file, 0, SEEK_END);
	size_t fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	size_t bufferLength = fileSize / sizeof(uint32_t);
	uint32_t* buffer = (uint32_t*)malloc(sizeof(uint32_t) * bufferLength);

	fread(buffer, sizeof(char), fileSize, file);
	fclose(file);

	VkShaderModuleCreateInfo createInfo = { 0 };
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = NULL;
	createInfo.codeSize = bufferLength * sizeof(uint32_t);
	createInfo.pCode = buffer;

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(vulkanContext.logicalDevice, &createInfo, NULL, &shaderModule) != VK_SUCCESS)
	{
		return false;
	}
	*outShaderModule = shaderModule;
	return true;
}

void InitPipelines()
{
	VkShaderModule meshFragShader;
	if (!LoadShaderModule("./Shaders/triangleFrag.spv", &meshFragShader))
	{
		printf("error when building the triangle fragment shader module\n");
	}
	
	VkShaderModule meshVertShader;
	if (!LoadShaderModule("./Shaders/meshTriangleVert.spv", &meshVertShader))
	{
		printf("error when building the triangle mesh shader module\n");
	}

	VkShaderModule guiVertShader;
	if (!LoadShaderModule("./Shaders/GUIVert.spv", &guiVertShader))
	{
		printf("error when building the GUI vert shader module\n");
	}
	
	VkShaderModule guiFragShader;
	if (!LoadShaderModule("./Shaders/GUIFrag.spv", &guiFragShader))
	{
		printf("error when building the GUI frag shader module\n");
	}
	
	struct PipelineBuilder pipelineBuilder;

	pipelineBuilder.shaderStagesLength = 2;
	pipelineBuilder.shaderStages = (VkPipelineShaderStageCreateInfo*)malloc(sizeof(VkPipelineShaderStageCreateInfo) * pipelineBuilder.shaderStagesLength);
	pipelineBuilder.shaderStages[0] = CreatePipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, meshVertShader);
	pipelineBuilder.shaderStages[1] = CreatePipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, meshFragShader);

	VkPipelineLayoutCreateInfo meshPipelineLayoutInfo = CreatePipelineLayoutCreateInfo();

	VkDescriptorSetLayout texturedSetLayouts[] = { vulkanContext.textureSetLayout };
	meshPipelineLayoutInfo.setLayoutCount = 1;
	meshPipelineLayoutInfo.pSetLayouts = texturedSetLayouts;

	VkPushConstantRange pushConstant;
	pushConstant.offset = 0;
	pushConstant.size = sizeof(struct MeshPushConstants);
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	meshPipelineLayoutInfo.pPushConstantRanges = &pushConstant;
	meshPipelineLayoutInfo.pushConstantRangeCount = 1;

	CheckVulkanResult(vkCreatePipelineLayout(vulkanContext.logicalDevice, &meshPipelineLayoutInfo, NULL, &vulkanContext.meshPipelineLayout), "Couldn't create pipeline layout");

	pipelineBuilder.pipelineLayout = vulkanContext.meshPipelineLayout;
	pipelineBuilder.vertexInputInfo = CreateVertexInputStateCreateInfo();
	pipelineBuilder.inputAssembly = CreateInputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);

	pipelineBuilder.viewport.x = 0.0f;
	pipelineBuilder.viewport.y = 0.0f;
	pipelineBuilder.viewport.width = (float)vulkanContext.windowExtent.width;
	pipelineBuilder.viewport.height = (float)vulkanContext.windowExtent.height;
	pipelineBuilder.viewport.minDepth = 0.0f;
	pipelineBuilder.viewport.maxDepth = 1.0f;

	pipelineBuilder.scissor.offset.x = 0;
	pipelineBuilder.scissor.offset.y = 0;
	pipelineBuilder.scissor.extent = vulkanContext.windowExtent;

	pipelineBuilder.rasterizer = CreateRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);

	pipelineBuilder.multisampling = CreateMultisamplingStateCreateInfo();

	pipelineBuilder.colorBlendAttachment = CreateColorBlendAttachmentState();

	struct VertexInputDescription vertexDescription = GetVertexDescription();

	pipelineBuilder.vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.attributes;
	pipelineBuilder.vertexInputInfo.vertexAttributeDescriptionCount = vertexDescription.attributesSize;

	pipelineBuilder.vertexInputInfo.pVertexBindingDescriptions = vertexDescription.bindings;
	pipelineBuilder.vertexInputInfo.vertexBindingDescriptionCount = vertexDescription.bindingsSize;

	vulkanContext.meshPipeline = BuildPipeline(vulkanContext.logicalDevice, vulkanContext.renderPass, pipelineBuilder);

	pipelineBuilder.shaderStages[0] = CreatePipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, guiVertShader);
	pipelineBuilder.shaderStages[1] = CreatePipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, guiFragShader);

	vulkanContext.guiPipeline = BuildPipeline(vulkanContext.logicalDevice, vulkanContext.renderPass, pipelineBuilder);

	vkDestroyShaderModule(vulkanContext.logicalDevice, meshVertShader, NULL);
	vkDestroyShaderModule(vulkanContext.logicalDevice, meshFragShader, NULL);
	vkDestroyShaderModule(vulkanContext.logicalDevice, guiVertShader, NULL);
	vkDestroyShaderModule(vulkanContext.logicalDevice, guiFragShader, NULL);
}

VkPipeline BuildPipeline(VkDevice device, VkRenderPass renderPass, struct PipelineBuilder pipelineBuilder)
{
	VkPipelineViewportStateCreateInfo viewportState = { 0 };
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.pNext = NULL;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &pipelineBuilder.viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &pipelineBuilder.scissor;

	VkPipelineColorBlendStateCreateInfo colorBlending = { 0 };
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.pNext = NULL;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &pipelineBuilder.colorBlendAttachment;
	colorBlending.blendConstants[0] = 1.0f;
	colorBlending.blendConstants[1] = 1.0f;
	colorBlending.blendConstants[2] = 1.0f;
	colorBlending.blendConstants[3] = 1.0f;

	VkGraphicsPipelineCreateInfo pipelineInfo = { 0 };
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = NULL;
	pipelineInfo.stageCount = pipelineBuilder.shaderStagesLength;
	pipelineInfo.pStages = pipelineBuilder.shaderStages;
	pipelineInfo.pVertexInputState = &pipelineBuilder.vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &pipelineBuilder.inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &pipelineBuilder.rasterizer;
	pipelineInfo.pMultisampleState = &pipelineBuilder.multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDepthStencilState = NULL;
	pipelineInfo.layout = pipelineBuilder.pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	
	VkPipeline newPipeline;
	if (vkCreateGraphicsPipelines(vulkanContext.logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &newPipeline) != VK_SUCCESS)
	{
		printf("failed to create pipeline\n");
		return VK_NULL_HANDLE;
	}
	else
	{
		return newPipeline;
	}
}

void InitMeshes()
{
	uint16_t width = vulkanContext.windowExtent.width;
	uint16_t height = vulkanContext.windowExtent.height;
	vulkanContext.spriteMesh.vertexSize = 4;

	vulkanContext.spriteMesh.vertices = (struct Vertex*)malloc(sizeof(struct Vertex) * vulkanContext.spriteMesh.vertexSize);

	//Because the viewport of a vulkan application goes from (-1, -1) to (1, 1), 32 pixels is actually 64 pixels.
	vulkanContext.spriteMesh.vertices[0].position[0] = -1.f + 64.f / width; //x (1.f)
	vulkanContext.spriteMesh.vertices[0].position[1] = -1.f + 64.f / height; //y (1.f)
	vulkanContext.spriteMesh.vertices[0].position[2] = 0.5f; //z

	vulkanContext.spriteMesh.vertices[1].position[0] = -1.f; //x (0.f)
	vulkanContext.spriteMesh.vertices[1].position[1] = -1.f + 64.f / height; //y (1.f)
	vulkanContext.spriteMesh.vertices[1].position[2] = 0.5f; //z

	vulkanContext.spriteMesh.vertices[2].position[0] = -1.f + 64.f / width; //x (1.f)
	vulkanContext.spriteMesh.vertices[2].position[1] = -1.f; //y (0.f)
	vulkanContext.spriteMesh.vertices[2].position[2] = 0.5f; //z
	
	vulkanContext.spriteMesh.vertices[3].position[0] = -1.f; //x (0.f)
	vulkanContext.spriteMesh.vertices[3].position[1] = -1.f; //y (0.f)
	vulkanContext.spriteMesh.vertices[3].position[2] = 0.5f; //z
	
	//in this one we don't need to multiply by 2, because the viewport goes from (0, 0) to (1, 1)
	vulkanContext.spriteMesh.vertices[0].textureUV[0] = 1.f; //x (1.f)
	vulkanContext.spriteMesh.vertices[0].textureUV[1] = 1.f; //y (1.f)

	vulkanContext.spriteMesh.vertices[1].textureUV[0] = 0.0f; //x (0.f)
	vulkanContext.spriteMesh.vertices[1].textureUV[1] = 1.f; //y (1.f)

	vulkanContext.spriteMesh.vertices[2].textureUV[0] = 1.f; //x (1.f)
	vulkanContext.spriteMesh.vertices[2].textureUV[1] = 0.0f; //y (0.f)

	vulkanContext.spriteMesh.vertices[3].textureUV[0] = 0.0f; //x (0.f)
	vulkanContext.spriteMesh.vertices[3].textureUV[1] = 0.0f; //y (0.f)


	UploadMesh(&vulkanContext.spriteMesh);

	vulkanContext.characterMesh.vertexSize = 4;

	vulkanContext.characterMesh.vertices = (struct Vertex*)malloc(sizeof(struct Vertex) * vulkanContext.characterMesh.vertexSize);

	vulkanContext.characterMesh.vertices[0].position[0] = -1.f + 32.f / width; //x (1.f)
	vulkanContext.characterMesh.vertices[0].position[1] = -1.f + 32.f / height; //y (1.f)
	vulkanContext.characterMesh.vertices[0].position[2] = 0.5f; //z

	vulkanContext.characterMesh.vertices[1].position[0] = -1.f; //x (0.f)
	vulkanContext.characterMesh.vertices[1].position[1] = -1.f + 32.f / height; //y (1.f)
	vulkanContext.characterMesh.vertices[1].position[2] = 0.5f; //z

	vulkanContext.characterMesh.vertices[2].position[0] = -1.f + 32.f / width; //x (1.f)
	vulkanContext.characterMesh.vertices[2].position[1] = -1.f; //y (0.f)
	vulkanContext.characterMesh.vertices[2].position[2] = 0.5f; //z

	vulkanContext.characterMesh.vertices[3].position[0] = -1.f; //x (0.f)
	vulkanContext.characterMesh.vertices[3].position[1] = -1.f; //y (0.f)
	vulkanContext.characterMesh.vertices[3].position[2] = 0.5f; //z

	//in this one we don't need to multiply by 2, because the viewport goes from (0, 0) to (1, 1)
	vulkanContext.characterMesh.vertices[0].textureUV[0] = 1.f; //x (1.f)
	vulkanContext.characterMesh.vertices[0].textureUV[1] = 1.f; //y (1.f)

	vulkanContext.characterMesh.vertices[1].textureUV[0] = 0.0f; //x (0.f)
	vulkanContext.characterMesh.vertices[1].textureUV[1] = 1.f; //y (1.f)

	vulkanContext.characterMesh.vertices[2].textureUV[0] = 1.f; //x (1.f)
	vulkanContext.characterMesh.vertices[2].textureUV[1] = 0.0f; //y (0.f)

	vulkanContext.characterMesh.vertices[3].textureUV[0] = 0.0f; //x (0.f)
	vulkanContext.characterMesh.vertices[3].textureUV[1] = 0.0f; //y (0.f)

	UploadMesh(&vulkanContext.characterMesh);
	vulkanContext.characterSize = 16;
}

void CreateMesh(ECS_ClickableScreenPosition_t* position, struct Mesh* mesh)
{
	uint16_t width = vulkanContext.windowExtent.width;
	uint16_t height = vulkanContext.windowExtent.height;
	mesh->vertexSize= 4;

	mesh->vertices = (struct Vertex*)malloc(sizeof(struct Vertex) * mesh->vertexSize);

	//Because the viewport of a vulkan application goes from (-1, -1) to (1, 1), 32 pixels is actually 64 pixels.
	mesh->vertices[0].position[0] = -1.f + ((float)(position->maxX << 1)) / width; //x (1.f)
	mesh->vertices[0].position[1] = -1.f + ((float)(position->maxY << 1)) / height; //y (1.f)
	mesh->vertices[0].position[2] = 0.5f; //z

	mesh->vertices[1].position[0] = -1.f + ((float)(position->minX << 1)) / width;  //x (0.f)
	mesh->vertices[1].position[1] = -1.f + ((float)(position->maxY << 1)) / height; //y (1.f)
	mesh->vertices[1].position[2] = 0.5f; //z

	mesh->vertices[2].position[0] = -1.f + ((float)(position->maxX << 1)) / width; //x (1.f)
	mesh->vertices[2].position[1] = -1.f + ((float)(position->minY << 1)) / height; //y (0.f)
	mesh->vertices[2].position[2] = 0.5f; //z

	mesh->vertices[3].position[0] = -1.f + ((float)(position->minX << 1)) / width; //x (0.f)
	mesh->vertices[3].position[1] = -1.f + ((float)(position->minY << 1)) / height; //y (0.f)
	mesh->vertices[3].position[2] = 0.5f; //z


	//in this one we don't need to multiply by 2, because the viewport goes from (0, 0) to (1, 1)
	mesh->vertices[0].textureUV[0] = 1.f; //x (1.f)
	mesh->vertices[0].textureUV[1] = 1.f; //y (1.f)

	mesh->vertices[1].textureUV[0] = 0.0f; //x (0.f)
	mesh->vertices[1].textureUV[1] = 1.f; //y (1.f)

	mesh->vertices[2].textureUV[0] = 1.f; //x (1.f)
	mesh->vertices[2].textureUV[1] = 0.0f; //y (0.f)

	mesh->vertices[3].textureUV[0] = 0.0f; //x (0.f)
	mesh->vertices[3].textureUV[1] = 0.0f; //y (0.f)


	UploadMesh(mesh);
}

void UploadMesh(struct Mesh* mesh)
{
	const size_t bufferSize = mesh->vertexSize * sizeof(struct Vertex);

	VkBufferCreateInfo stagingBufferInfo = { 0 };
	stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferInfo.pNext = NULL;
	stagingBufferInfo.size = bufferSize;
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VmaAllocationCreateInfo vmaAllocInfo = { 0 };
	vmaAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

	struct AllocatedBuffer stagingBuffer;
	CheckVulkanResult(vmaCreateBuffer(vulkanContext.allocator, &stagingBufferInfo, &vmaAllocInfo, &stagingBuffer.buffer, &stagingBuffer.allocation, NULL), "couldn't create vertex buffer\n");

	void* data;
	vmaMapMemory(vulkanContext.allocator, stagingBuffer.allocation, &data);
	memcpy(data, mesh->vertices, mesh->vertexSize * sizeof(struct Vertex));
	vmaUnmapMemory(vulkanContext.allocator, stagingBuffer.allocation);

	VkBufferCreateInfo vertexBufferInfo = { 0 };
	vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vertexBufferInfo.pNext = NULL;
	vertexBufferInfo.size = bufferSize;
	vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	vmaAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	CheckVulkanResult(vmaCreateBuffer(vulkanContext.allocator, &vertexBufferInfo, &vmaAllocInfo, &mesh->vertexBuffer.buffer, &mesh->vertexBuffer.allocation, NULL), "couldn't create buffer");

	BufferImmediateSubmit(bufferSize, &stagingBuffer, mesh, false, NULL, NULL);

	vmaDestroyBuffer(vulkanContext.allocator, stagingBuffer.buffer, stagingBuffer.allocation);
}

static struct VertexInputDescription GetVertexDescription()
{
	struct VertexInputDescription description;

	VkVertexInputBindingDescription mainBinding = { 0 };
	mainBinding.binding = 0;
	mainBinding.stride = sizeof(struct Vertex);
	mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	description.bindingsSize = 1;
	description.bindings = (VkVertexInputBindingDescription*)malloc(sizeof(VkVertexInputBindingDescription) * description.bindingsSize);
	description.bindings[0] = mainBinding;

	VkVertexInputAttributeDescription positionAttribute = { 0 };
	positionAttribute.binding = 0;
	positionAttribute.location = 0;
	positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionAttribute.offset = offsetof(struct Vertex, position);

	VkVertexInputAttributeDescription normalAttribute = { 0 };
	normalAttribute.binding = 0;
	normalAttribute.location = 1;
	normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
	normalAttribute.offset = offsetof(struct Vertex, normal);

	VkVertexInputAttributeDescription uvAttribute = { 0 };
	uvAttribute.binding = 0;
	uvAttribute.location = 2;
	uvAttribute.format = VK_FORMAT_R32G32_SFLOAT;
	uvAttribute.offset = offsetof(struct Vertex, textureUV);

	description.attributesSize = 3;
	description.attributes = (VkVertexInputAttributeDescription*)malloc(sizeof(VkVertexInputAttributeDescription) * description.attributesSize);
	description.attributes[0] = positionAttribute;
	description.attributes[1] = normalAttribute;
	description.attributes[2] = uvAttribute;

	return description;
}

struct FrameData* GetCurrentFrame()
{
	return &vulkanContext.frames[vulkanContext.frameNumber % FRAME_OVERLAP];
}

uint32_t InitiateDrawCalls()
{
	struct FrameData* frameData = GetCurrentFrame();
	

	// 1000000000 is 1 second, vkWaitForFences uses nanoseconds
	CheckVulkanResult(vkWaitForFences(vulkanContext.logicalDevice, 1, &frameData->renderFence, true, 1000000000), "Error in fence");
	CheckVulkanResult(vkResetFences(vulkanContext.logicalDevice, 1, &frameData->renderFence), "Couldn't reset fences");

	uint32_t swapchainImageIndex;
	CheckVulkanResult(vkAcquireNextImageKHR(vulkanContext.logicalDevice, vulkanContext.swapChain, 1000000000, frameData->presentSemaphore, NULL, &swapchainImageIndex), "Error when acquiring next image");

	CheckVulkanResult(vkResetCommandBuffer(frameData->commandBuffer, 0), "couldn't reset command buffer");

	VkCommandBuffer commandBuffer = frameData->commandBuffer;

	VkCommandBufferBeginInfo commandBufferBeginInfo = { 0 };
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = NULL;
	commandBufferBeginInfo.pInheritanceInfo = NULL;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	CheckVulkanResult(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo), "couldn't begin command buffer");
	int value = vulkanContext.frameNumber;
	VkClearValue clearValue = { 0.0f, 0.0f, 0.f, 1.0f };
	/*float flash = fabs(sin(value / 120.f));
	float tempFloat[] = { 0.0f, 0.0f, flash, 1.0f };
	memcpy(clearValue.color.float32, tempFloat, sizeof(float) * 4);*/

	VkClearValue clearValues[] = { clearValue };

	VkRenderPassBeginInfo renderPassBeginInfo = { 0 };
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = NULL;
	renderPassBeginInfo.renderPass = vulkanContext.renderPass;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent = vulkanContext.windowExtent;
	renderPassBeginInfo.framebuffer = vulkanContext.framebuffers[swapchainImageIndex];
	renderPassBeginInfo.clearValueCount = 1;
	renderPassBeginInfo.pClearValues = &clearValues[0];

	//It's here that we will pass the commands to the pipeline.

	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	//save the current frame and get the updated command buffer.
	vulkanContext.currentFrame = *frameData;
	vulkanContext.currentFrame.commandBuffer = commandBuffer;

	return swapchainImageIndex;
}

void BindSpriteSheet(VkCommandBuffer commandBuffer)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanContext.meshPipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanContext.meshPipelineLayout, 0, 1, &vulkanContext.textureSet, 0, NULL);
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vulkanContext.spriteMesh.vertexBuffer.buffer, &offset);
}

void BindCharacterSheet(VkCommandBuffer commandBuffer)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanContext.meshPipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanContext.meshPipelineLayout, 0, 1, &vulkanContext.characterSet, 0, NULL);
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vulkanContext.characterMesh.vertexBuffer.buffer, &offset);
}

void BindGuiPipeline(VkCommandBuffer commandBuffer, ECS_ClickableScreenPosition_t* position)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanContext.guiPipeline);
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vulkanContext.guiMesh.vertexBuffer.buffer, &offset);
}

void DrawMap(VkCommandBuffer commandBuffer, struct MapHelper* map)
{
	struct MeshPushConstants constants;
	glm_vec2_copy((vec2) { vulkanContext.windowExtent.width, vulkanContext.windowExtent.height }, constants.screenDimensions);
	//the screen goes from -1 to 1, so the size of the tile needs to be the size * 2;
	constants.sizeOfTile = 32 << 1;
	for (uint8_t yTilePos = 0; yTilePos < 15; yTilePos++)
	{
		for (uint8_t xTilePos = 0; xTilePos < 20; xTilePos++)
		{
			constants.characterLayer = map->map[map->renderPortion.minY + yTilePos][map->renderPortion.minX + xTilePos];
			glm_vec2_copy((vec2) { xTilePos, yTilePos }, constants.tileOffset);

			vkCmdPushConstants(commandBuffer, vulkanContext.meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(struct MeshPushConstants), &constants);

			vkCmdDraw(commandBuffer, vulkanContext.spriteMesh.vertexSize, 1, 0, 0);
		}
	}
}

void DrawSquare(VkCommandBuffer commandBuffer)
{
	struct MeshPushConstants constants = { 0 };
	constants.guiAnimationTime = fabs(sin(vulkanContext.frameNumber / 15.f));
	vkCmdPushConstants(commandBuffer, vulkanContext.meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(struct MeshPushConstants), &constants);
	vkCmdDraw(commandBuffer, vulkanContext.guiMesh.vertexSize, 1, 0, 0);
}

void DrawEntity(VkCommandBuffer commandBuffer, uint16_t tileNumber, uint8_t xPos, uint8_t yPos)
{
	struct MeshPushConstants constants;
	glm_vec2_copy((vec2) { vulkanContext.windowExtent.width, vulkanContext.windowExtent.height }, constants.screenDimensions);
	glm_vec2_copy((vec2) { xPos, yPos }, constants.tileOffset);
	constants.characterLayer = tileNumber;
	//the screen goes from -1 to 1, so the size of the tile needs to be the size * 2;
	constants.sizeOfTile = (32 << 1);

	vkCmdPushConstants(commandBuffer, vulkanContext.meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(struct MeshPushConstants), &constants);

	vkCmdDraw(commandBuffer, vulkanContext.spriteMesh.vertexSize, 1, 0, 0);
}

void DrawString(VkCommandBuffer commandBuffer, char* string, uint16_t xPos, uint16_t yPos)
{
	struct MeshPushConstants constants = { 0 };
	glm_vec2_copy((vec2) { vulkanContext.windowExtent.width, vulkanContext.windowExtent.height }, constants.screenDimensions);
	uint8_t stringLen = strlen((const char*)string);
	//the screen goes from -1 to 1, so the size of the tile needs to be the size * 2;
	constants.sizeOfTile = (vulkanContext.characterSize << 1);

	for (uint8_t xDelta = 0; xDelta < stringLen; xDelta++)
	{
		glm_vec2_copy((vec2) { xPos + xDelta, yPos }, constants.tileOffset);

		constants.characterLayer = string[xDelta];
		vkCmdPushConstants(commandBuffer, vulkanContext.meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(struct MeshPushConstants), &constants);

		vkCmdDraw(commandBuffer, vulkanContext.characterMesh.vertexSize, 1, 0, 0);
	}
}

void SubmitDrawCalls(uint32_t swapchainImageIndex)
{
	
	
	struct FrameData* frameData = &vulkanContext.currentFrame;
	VkCommandBuffer commandBuffer = frameData->commandBuffer;


	vkCmdEndRenderPass(commandBuffer);

	CheckVulkanResult(vkEndCommandBuffer(commandBuffer), "couldn't end command buffer");

	VkSubmitInfo submit = { 0 };
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.pNext = NULL;
	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submit.pWaitDstStageMask = &waitStage;
	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &frameData->presentSemaphore;
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &frameData->renderSemaphore;
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &commandBuffer;

	CheckVulkanResult(vkQueueSubmit(vulkanContext.graphicsQueue, 1, &submit, frameData->renderFence), "couldn't submit to the queue");

	VkPresentInfoKHR presentInfo = { 0 };
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = NULL;
	presentInfo.pSwapchains = &vulkanContext.swapChain;
	presentInfo.swapchainCount = 1;
	presentInfo.pWaitSemaphores = &frameData->renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pImageIndices = &swapchainImageIndex;

	CheckVulkanResult(vkQueuePresentKHR(vulkanContext.graphicsQueue, &presentInfo), "couldn't present image");

	vulkanContext.frameNumber++;
}

void InitDescriptorSets()
{
	//Sprite sheet
	VkSamplerCreateInfo samplerInfo = CreatesamplerCreateInfo(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT);

	VkSampler spriteSheetSampler;
	vkCreateSampler(vulkanContext.logicalDevice, &samplerInfo, NULL, &spriteSheetSampler);

	VkDescriptorSetAllocateInfo allocInfo = { 0 };
	allocInfo.pNext = NULL;
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = vulkanContext.descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &vulkanContext.textureSetLayout;

	vkAllocateDescriptorSets(vulkanContext.logicalDevice, &allocInfo, &vulkanContext.textureSet);
	VkDescriptorImageInfo spriteBufferInfo;
	spriteBufferInfo.sampler = spriteSheetSampler;
	spriteBufferInfo.imageView = vulkanContext.loadedTextures[SpriteSheetIndex].imageView;
	spriteBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet spriteSheetWriteDescriptorSet = CreatewriteDescriptorImage(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, vulkanContext.textureSet, &spriteBufferInfo, 0);
	vkUpdateDescriptorSets(vulkanContext.logicalDevice, 1, &spriteSheetWriteDescriptorSet, 0, NULL);



	vkAllocateDescriptorSets(vulkanContext.logicalDevice, &allocInfo, &vulkanContext.characterSet);

	spriteBufferInfo.imageView = vulkanContext.loadedTextures[CharacterSheetIndex].imageView;
	VkWriteDescriptorSet characterWriteDescriptorSet = CreatewriteDescriptorImage(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, vulkanContext.characterSet, &spriteBufferInfo, 0);

	vkUpdateDescriptorSets(vulkanContext.logicalDevice, 1, &characterWriteDescriptorSet, 0, NULL);
}

void InitDescriptors()
{
	VkDescriptorPoolSize sizes[] =
	{
		{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10}
	};

	VkDescriptorPoolCreateInfo poolInfo = { 0 };
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = 0;
	poolInfo.maxSets = 10;
	poolInfo.poolSizeCount = (uint32_t)(sizeof(sizes) / sizeof(VkDescriptorPoolSize));
	poolInfo.pPoolSizes = sizes;

	vkCreateDescriptorPool(vulkanContext.logicalDevice, &poolInfo, NULL, &vulkanContext.descriptorPool);

	VkDescriptorSetLayoutBinding textureBinding = CreateDescriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0);
	
	VkDescriptorSetLayoutCreateInfo textureInfo = { 0 };
	textureInfo.bindingCount = 1;
	textureInfo.flags = 0;
	textureInfo.pNext = NULL;
	textureInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	textureInfo.pBindings = &textureBinding;

	vkCreateDescriptorSetLayout(vulkanContext.logicalDevice, &textureInfo, NULL, &vulkanContext.textureSetLayout);
}

void BufferImmediateSubmit(const size_t bufferSize, struct AllocatedBuffer* stagingBuffer, struct Mesh* mesh, bool uploadingTexture, struct AllocatedImage* newImage, VkExtent3D* imageExtent)
{
	VkCommandBufferAllocateInfo commandBufferAllocInfo = CreateCommandBufferAllocateInfo(vulkanContext.uploadContext.commandPool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	VkCommandBuffer commandBuffer;
	CheckVulkanResult(vkAllocateCommandBuffers(vulkanContext.logicalDevice, &commandBufferAllocInfo, &commandBuffer), "couldn't allocate command buffer\n");

	VkCommandBufferBeginInfo commandBufferBeginInfo = CreateCommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	CheckVulkanResult(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo), "couldn't begin command buffer\n");

	//execute the function
	if (!uploadingTexture)
	{
		VkBufferCopy copy;
		copy.dstOffset = 0;
		copy.srcOffset = 0;
		copy.size = bufferSize;
		vkCmdCopyBuffer(commandBuffer, stagingBuffer->buffer, mesh->vertexBuffer.buffer, 1, &copy);
	}
	else
	{
		VkImageSubresourceRange range ;
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel = 0;
		range.levelCount = 1;
		range.baseArrayLayer = 0;
		range.layerCount = 1;

		VkImageMemoryBarrier imageBarrierToTransfer = { 0 };
		imageBarrierToTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageBarrierToTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageBarrierToTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrierToTransfer.image = newImage->image;
		imageBarrierToTransfer.subresourceRange = range;
		imageBarrierToTransfer.srcAccessMask = 0;
		imageBarrierToTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &imageBarrierToTransfer);
	
		VkBufferImageCopy copyRegion = { 0 };
		copyRegion.bufferOffset = 0;
		copyRegion.bufferRowLength = 0;
		copyRegion.bufferImageHeight = 0;
		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel = 0;
		copyRegion.imageSubresource.baseArrayLayer = 0;
		copyRegion.imageSubresource.layerCount = 1;
		copyRegion.imageExtent = *imageExtent;

		vkCmdCopyBufferToImage(commandBuffer, stagingBuffer->buffer, newImage->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

		VkImageMemoryBarrier imageBarrierToReadeable = imageBarrierToTransfer;
		imageBarrierToReadeable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrierToReadeable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageBarrierToReadeable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageBarrierToReadeable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &imageBarrierToReadeable);
	}

	CheckVulkanResult(vkEndCommandBuffer(commandBuffer), "couldn't end command buffer\n");

	VkSubmitInfo submit = CreateSubmitInfo(&commandBuffer);
	CheckVulkanResult(vkQueueSubmit(vulkanContext.graphicsQueue, 1, &submit, vulkanContext.uploadContext.uploadFence), "couldn't submit queue\n");
	
	vkWaitForFences(vulkanContext.logicalDevice, 1, &vulkanContext.uploadContext.uploadFence, true, 9999999999);
	vkResetFences(vulkanContext.logicalDevice, 1, &vulkanContext.uploadContext.uploadFence);

	vkResetCommandPool(vulkanContext.logicalDevice, vulkanContext.uploadContext.commandPool, 0);
}

struct AllocatedBuffer CreateBuffer(size_t AllocationSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
{
	VkBufferCreateInfo bufferInfo = { 0 };
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = NULL;
	bufferInfo.size = AllocationSize;
	bufferInfo.usage = usage;

	VmaAllocationCreateInfo vmaAllocationInfo = { 0 };
	vmaAllocationInfo.usage = memoryUsage;

	struct AllocatedBuffer newBuffer;

	CheckVulkanResult(vmaCreateBuffer(vulkanContext.allocator, &bufferInfo, &vmaAllocationInfo, &newBuffer.buffer, &newBuffer.allocation, NULL), "Couldn't create buffer");
	return newBuffer;
}

void ImageImmediateSubmit(struct AllocatedBuffer* stagingBuffer, struct AllocatedImage* newImage, VkExtent3D* imageExtent, uint32_t layerOffset, uint32_t arrayLayers)
{
	VkCommandBufferAllocateInfo commandBufferAllocInfo = CreateCommandBufferAllocateInfo(vulkanContext.uploadContext.commandPool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	VkCommandBuffer commandBuffer;
	CheckVulkanResult(vkAllocateCommandBuffers(vulkanContext.logicalDevice, &commandBufferAllocInfo, &commandBuffer), "couldn't allocate command buffer\n");

	VkCommandBufferBeginInfo commandBufferBeginInfo = CreateCommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	CheckVulkanResult(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo), "couldn't begin command buffer\n");

	VkImageSubresourceRange range = { 0 };
	range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	range.baseMipLevel = 0;
	range.levelCount = 1;
	range.layerCount = arrayLayers;

	VkImageMemoryBarrier imageBarrierToTransfer = { 0 };
	imageBarrierToTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageBarrierToTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageBarrierToTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageBarrierToTransfer.image = newImage->image;
	imageBarrierToTransfer.subresourceRange = range;
	imageBarrierToTransfer.srcAccessMask = 0;
	imageBarrierToTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &imageBarrierToTransfer);

	VkBufferImageCopy* bufferCopyRegions = (VkBufferImageCopy*)malloc(sizeof(VkBufferImageCopy) * arrayLayers);

	for (uint32_t layer = 0; layer < arrayLayers; layer++)
	{
		uint32_t offset = layerOffset * layer;

		VkBufferImageCopy copyRegion = { 0 };
		copyRegion.bufferOffset = offset;
		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel = 0;
		copyRegion.imageSubresource.baseArrayLayer = layer;
		copyRegion.imageSubresource.layerCount = 1;
		copyRegion.imageExtent = *imageExtent;

		bufferCopyRegions[layer] = copyRegion;
	}


	vkCmdCopyBufferToImage(commandBuffer, stagingBuffer->buffer, newImage->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, arrayLayers, bufferCopyRegions);

	VkImageMemoryBarrier imageBarrierToReadeable = imageBarrierToTransfer;
	imageBarrierToReadeable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	imageBarrierToReadeable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageBarrierToReadeable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	imageBarrierToReadeable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &imageBarrierToReadeable);

	CheckVulkanResult(vkEndCommandBuffer(commandBuffer), "couldn't end command buffer\n");

	VkSubmitInfo submit = CreateSubmitInfo(&commandBuffer);
	CheckVulkanResult(vkQueueSubmit(vulkanContext.graphicsQueue, 1, &submit, vulkanContext.uploadContext.uploadFence), "couldn't submit queue\n");

	vkWaitForFences(vulkanContext.logicalDevice, 1, &vulkanContext.uploadContext.uploadFence, true, 9999999999);
	vkResetFences(vulkanContext.logicalDevice, 1, &vulkanContext.uploadContext.uploadFence);

	vkResetCommandPool(vulkanContext.logicalDevice, vulkanContext.uploadContext.commandPool, 0);
}

bool loadImageFromFile(const char* file, struct AllocatedImage* outImage, uint32_t arrayLayers, struct TextureArrayDimensions textArrDimensions, VkExtent3D imageExtent)
{
	int textureWidth;
	int textureHeight;
	int textureChannels;

	stbi_uc* pixels = stbi_load(file, &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);

	if (!pixels)
	{
		printf("couldn't load texture file %s\n", file);
		return false;
	}

	/*
	Translating the texture atlas to texture array format.To do this, the pixels of every texture of the texture atlas need to be inside the offset.
	The memory layout of the stbi_load puts the bytes line by line, so for example if we have two 2width x 2height texture, with red and alpha channels, the memory layout will go like this:
	   
	   23-255   80-255      50-255  32-255
	   202-255  255-255     22-0    50-255          -> array[] = { 23-255, 80-255, 50-255, 32-255, 202-255, 255-255, 22-0, 50-255 }
	
	Where the first texture is { 23-255, 80-255, 202-255, 255-255 } and the second texture is { 50-255, 32-255, 22-0, 50-255 }.

	In a texture array we need to store every byte of a single texture one after another, so instead of having the array[] like before, we need to have something like this:
	-> array[] = { 23-255, 80-255, 202-255, 255-255 } <- first texture        +       { 50-255, 32-255, 22-0, 50-255 } <- second texture
	
	In other words, we will fill the array with the bytes of the texture from left to right, top to down.
	*/
	uint8_t* newPixels = (uint8_t*)malloc(sizeof(uint8_t) * textureChannels * textureHeight * textureWidth);
	size_t count = 0;

	uint32_t maxPixelsPerHorizontalLine = textArrDimensions.texturesPerCol * textArrDimensions.RGBA_PerHorizontalLine;
	for (uint32_t textureRowindex = 0; textureRowindex < textArrDimensions.texturesPerRow; textureRowindex++)
	{
		for(uint32_t textureColIndex = 0; textureColIndex < textArrDimensions.texturesPerCol; textureColIndex++)
		{
			for (uint8_t pixelVerticalIndex = 0; pixelVerticalIndex < textArrDimensions.pixelsPerVerticalLine; pixelVerticalIndex++)
			{
				for (uint8_t pixelHorizontalIndex = 0; pixelHorizontalIndex < textArrDimensions.RGBA_PerHorizontalLine; pixelHorizontalIndex++)
				{

					newPixels[count] = pixels[((textureColIndex * textArrDimensions.RGBA_PerHorizontalLine) + pixelHorizontalIndex)  + 
						(((textureRowindex * textArrDimensions.pixelsPerVerticalLine) * maxPixelsPerHorizontalLine) + (pixelVerticalIndex * maxPixelsPerHorizontalLine))];
					count++;
				}
			}	
		}
	}

	void* pixelPointer = newPixels;
	VkDeviceSize imageSize = textureWidth * textureHeight * 4;

	struct AllocatedBuffer stagingBuffer = CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	void* data;
	vmaMapMemory(vulkanContext.allocator, stagingBuffer.allocation, &data);
	memcpy(data, pixelPointer, (size_t)imageSize);
	vmaUnmapMemory(vulkanContext.allocator, stagingBuffer.allocation);

	stbi_image_free(pixels);

	//how many textures there are in the texture array and the byte offset that the gpu needs to jump to get the next texture.
	uint32_t layerOffset = imageExtent.width * imageExtent.height * 4;

	VkImageCreateInfo dimgInfo = CreateImageCreateInfo(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, imageExtent, arrayLayers);

	struct AllocatedImage newImage;

	VmaAllocationCreateInfo dimgAllocInfo = { 0 };
	dimgAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	vmaCreateImage(vulkanContext.allocator, &dimgInfo, &dimgAllocInfo, &newImage.image, &newImage.allocation, NULL);

	ImageImmediateSubmit(&stagingBuffer, &newImage, &imageExtent, layerOffset, arrayLayers);

	vmaDestroyBuffer(vulkanContext.allocator, stagingBuffer.buffer, stagingBuffer.allocation);

	*outImage = newImage;
	return true;
}

void ChangeVertexBuffer()
{
	CreateMesh(8, &vulkanContext.guiMesh);
}

void loadImages()
{
	struct Texture spriteSheet;
	VkExtent3D spriteSheetImageExtent = { 32, 32, 1 };
	struct TextureArrayDimensions spriteSheetDimensions = { 48, 22, 128, 32 };
	loadImageFromFile("./assets/colored_packed.png", &spriteSheet.image, 1056, spriteSheetDimensions, spriteSheetImageExtent);
	VkImageViewCreateInfo imageInfo = CreateImageViewCreateInfo(VK_FORMAT_R8G8B8A8_SRGB, spriteSheet.image.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D_ARRAY, 1056);
	vkCreateImageView(vulkanContext.logicalDevice, &imageInfo, NULL, &spriteSheet.imageView);
	
	struct Texture characterSheet;
	struct TextureArrayDimensions characterGlyphsDimensions = { 16, 16, 32, 8 };
	VkExtent3D characterGlyphsImageExtent = { 8, 8, 1 };
	loadImageFromFile("./assets/glyphs.png", &characterSheet.image, 256, characterGlyphsDimensions, characterGlyphsImageExtent);
	VkImageViewCreateInfo imageInfo2 = CreateImageViewCreateInfo(VK_FORMAT_R8G8B8A8_SRGB, characterSheet.image.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D_ARRAY, 256);
	vkCreateImageView(vulkanContext.logicalDevice, &imageInfo2, NULL, &characterSheet.imageView);

	vulkanContext.loadedTexturesSize = 2;
	vulkanContext.loadedTextures = (struct Texture*)malloc(sizeof(struct Texture) * vulkanContext.loadedTexturesSize);
	vulkanContext.loadedTextures[SpriteSheetIndex] = spriteSheet;
	vulkanContext.loadedTextures[CharacterSheetIndex] = characterSheet;
}

void CleanupVulkan()
{
	if (vulkanContext.isInitialized)
	{
		DeletionQueue();

		vkDestroySurfaceKHR(vulkanContext.instance, vulkanContext.surface, NULL);
		vkDestroyDevice(vulkanContext.logicalDevice, NULL);
		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(vulkanContext.instance, vulkanContext.debugMessenger, NULL);
		}
		vkDestroyInstance(vulkanContext.instance, NULL);
	}
}

void DeletionQueue()
{
	vulkanContext.frameNumber--;
	struct FrameData* framedata = GetCurrentFrame();
	vulkanContext.frameNumber++;
	vkWaitForFences(vulkanContext.logicalDevice, 1, &(*framedata).renderFence, true, 1000000000);
	
	vmaDestroyImage(vulkanContext.allocator, vulkanContext.loadedTextures[SpriteSheetIndex].image.image, vulkanContext.loadedTextures[SpriteSheetIndex].image.allocation);
	vmaDestroyImage(vulkanContext.allocator, vulkanContext.loadedTextures[CharacterSheetIndex].image.image, vulkanContext.loadedTextures[CharacterSheetIndex].image.allocation);

	for (uint16_t i = 0; i < FRAME_OVERLAP; i++)
	{
		vkDestroyCommandPool(vulkanContext.logicalDevice, vulkanContext.frames[i].commandPool, NULL);

		vkDestroyFence(vulkanContext.logicalDevice, vulkanContext.frames[i].renderFence, NULL);
		vkDestroySemaphore(vulkanContext.logicalDevice, vulkanContext.frames[i].renderSemaphore, NULL);
		vkDestroySemaphore(vulkanContext.logicalDevice, vulkanContext.frames[i].presentSemaphore, NULL);
	}
	vkDestroyCommandPool(vulkanContext.logicalDevice, vulkanContext.uploadContext.commandPool, NULL);
	vkDestroyFence(vulkanContext.logicalDevice, vulkanContext.uploadContext.uploadFence, NULL);

	vkDestroyRenderPass(vulkanContext.logicalDevice, vulkanContext.renderPass, NULL);

	vkDestroySwapchainKHR(vulkanContext.logicalDevice, vulkanContext.swapChain, NULL);
	for (uint16_t imageViewIndex = 0; imageViewIndex < vulkanContext.swapChainImagesCount; imageViewIndex++)
	{
		vkDestroyFramebuffer(vulkanContext.logicalDevice, vulkanContext.framebuffers[imageViewIndex], NULL);
		vkDestroyImageView(vulkanContext.logicalDevice, vulkanContext.swapChainImageViews[imageViewIndex], NULL);
	}

	vkDestroyPipeline(vulkanContext.logicalDevice, vulkanContext.meshPipeline, NULL);
	vkDestroyPipelineLayout(vulkanContext.logicalDevice, vulkanContext.meshPipelineLayout, NULL);

	vmaDestroyBuffer(vulkanContext.allocator, vulkanContext.spriteMesh.vertexBuffer.buffer, vulkanContext.spriteMesh.vertexBuffer.allocation);
	vmaDestroyBuffer(vulkanContext.allocator, vulkanContext.characterMesh.vertexBuffer.buffer, vulkanContext.characterMesh.vertexBuffer.allocation);
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

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != NULL) {
		func(instance, debugMessenger, pAllocator);
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

void* maxNumber(void* a, void* b)
{
	return a > b ? a : b;
}

void* minNumber(void* a, void* b)
{
	return a > b ? b : a;
}








//Unused but maybe useful later.



//The freetype characters are initialized as a texture array.
/*void InitFreeType(struct AllocatedImage* outImage)
{

	if (FT_Init_FreeType(&freeTypeHelper.library))
	{
		printf("couldn't initialize Freetype\n");
		exit(1);
	}

	if (FT_New_Face(freeTypeHelper.library, "./Assets/Early GameBoy.ttf", 0, &freeTypeHelper.face))
	{
		printf("couldn't open font\n");
		exit(1);
	}

	FT_Set_Pixel_Sizes(freeTypeHelper.face, 0, 16);

	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t layers = 0;
	uint32_t layerOffset;
	for (uint32_t i = 'A'; i <= 'Z'; i++)
	{
		if (FT_Load_Char(freeTypeHelper.face, i, FT_LOAD_RENDER))
		{
			printf("couldn't load character\n");
			exit(1);
		}

		width = maxNumber(freeTypeHelper.face->glyph->bitmap.width, width);
		height = maxNumber(freeTypeHelper.face->glyph->bitmap.rows, height);
		layers++;
	}

	vulkanContext.characterLayerCount = layers;

	VkDeviceSize imageSize = (width * layers) * height * 4;
	layerOffset = width * height * 4;
	uint8_t* RGB_CharacterBuffer = (uint8_t*)calloc(imageSize, sizeof(uint8_t));

	for (uint32_t i = 'A'; i <= 'Z'; i++)
	{
		if (FT_Load_Char(freeTypeHelper.face, i, FT_LOAD_RENDER))
		{
			printf("couldn't load character\n");
			exit(1);
		}

		freeTypeHelper.glyph = freeTypeHelper.face->glyph;

		size_t RGBIndex = layerOffset * (i - 'A');
		uint8_t* characterBuffer = freeTypeHelper.glyph->bitmap.buffer;

		for (int characterBufferIndex = 0; characterBufferIndex < freeTypeHelper.glyph->bitmap.rows * freeTypeHelper.glyph->bitmap.width; characterBufferIndex++) {

			RGB_CharacterBuffer[RGBIndex] = 0xFF;
			RGBIndex++;
			RGB_CharacterBuffer[RGBIndex] = 0xFF;
			RGBIndex++;
			RGB_CharacterBuffer[RGBIndex] = 0xFF;
			RGBIndex++;
			RGB_CharacterBuffer[RGBIndex] = characterBuffer[characterBufferIndex];
			RGBIndex++;
		}
	}


	VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
	void* pixelPointer = RGB_CharacterBuffer;
	struct AllocatedBuffer stagingBuffer = CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	void* data;
	vmaMapMemory(vulkanContext.allocator, stagingBuffer.allocation, &data);
	memcpy(data, pixelPointer, (size_t)imageSize);
	vmaUnmapMemory(vulkanContext.allocator, stagingBuffer.allocation);

	VkExtent3D imageExtent;
	imageExtent.width = (uint32_t)freeTypeHelper.glyph->bitmap.width;
	imageExtent.height = (uint32_t)freeTypeHelper.glyph->bitmap.rows;
	imageExtent.depth = 1;

	VkImageCreateInfo dimgInfo = { 0 };
	dimgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	dimgInfo.imageType = VK_IMAGE_TYPE_2D;
	dimgInfo.format = imageFormat;
	dimgInfo.mipLevels = 1;
	dimgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	dimgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	dimgInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	dimgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	dimgInfo.extent = imageExtent;
	dimgInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	dimgInfo.arrayLayers = vulkanContext.characterLayerCount;

	struct AllocatedImage newImage;

	VmaAllocationCreateInfo dimgAllocInfo = { 0 };
	dimgAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	vmaCreateImage(vulkanContext.allocator, &dimgInfo, &dimgAllocInfo, &newImage.image, &newImage.allocation, NULL);

	ImageImmediateSubmit(&stagingBuffer, &newImage, &imageExtent, layerOffset);

	vmaDestroyBuffer(vulkanContext.allocator, stagingBuffer.buffer, stagingBuffer.allocation);

	*outImage = newImage;
}*/
