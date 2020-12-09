#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//need to wait for the next microsoft windows update to make it included in visual studio.
#include "stdalign.h"

#include <cglm/cglm.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "VulkanNuklear.h"


//vulkan specific
struct vulkan_context
{
	VkInstance instance;

	VkLayerProperties* validationLayers;
	uint32_t validationLayersLength;
	uint32_t enabledLayersLength;

	uint32_t glfwExtensionCount;
	const char** glfwExtensions;

	VkSurfaceKHR surface;

	struct QueueFamilyIndices
	{
		uint32_t graphicsFamily;
		bool graphicsFamilyFound;

		//queue family that has the capability of presenting to window surface.
		uint32_t presentFamily;
		bool presentFamilyFound;
	}familyIndices;

	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties physicalDeviceProperties;
	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

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

	struct VertFragBufferDetails
	{
		size_t vertexShaderBufferLength;
		size_t fragmentShaderBufferLength;
	}vertFragBufferDetails;

	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	VkFramebuffer* swapChainFrameBuffers;

	VkCommandPool commandPool;
	VkCommandBuffer* commandBuffers;

	VkSemaphore* imageAvailableSemaphore;
	VkSemaphore* renderFinishedSemaphore;
	VkFence* inFlightFences;
	VkFence* imagesInFlight;
	size_t currentFrame;

	bool framebufferResized;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	VkBuffer* uniformBuffers;
	VkDeviceMemory* uniformBuffersMemory;

	VkDescriptorPool descriptorPool;
	VkDescriptorSet* descriptorSets;

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;

	VkDebugUtilsMessengerEXT debugMessenger;
}context;

struct Vertex
{
	vec2 pos;
	vec3 color;
	vec2 textureCoordinate;
};

struct VertexHelper
{
	size_t verticesSize;
	size_t vulkanBindingDescriptionSize;
	size_t vulkanAttributeDescriptionSize;
	size_t indicesSize;
};

struct UniformBufferObject
{
	alignas(16) mat4 model;
	alignas(16) mat4 view;
	alignas(16) mat4 proj;
};

extern double startTime;

//vertex
extern const struct Vertex* vertices;
extern struct VertexHelper vertexHelper;
//indices
extern const uint16_t* indices;

//window specific
extern const uint32_t WIDTH;
extern const uint32_t HEIGHT;
extern const char* engineName;
extern GLFWwindow* window;

//vulkan validation layers names
extern const char* validationLayersNames[];

extern const char* deviceExtensions[];

extern const int MAX_FRAMES_IN_FLIGHT;



#ifdef NDEBUG
extern const bool enableValidationLayers;
#else
extern const bool enableValidationLayers;
#endif

void PrepareVertices();
static VkVertexInputBindingDescription getBindingDescription();
static VkVertexInputAttributeDescription* getAttributeDescriptions();
void CreateVertexBuffer();
void CreateIndexBuffer();
void CreateDescriptorSetLayout();
void CreateUniformBuffers();
void UpdateUniformBuffer(uint32_t currentImage);
void CreateDescriptorPool();
void CreateDescriptorSets();
uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
void Run();
void InitWindow();
static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
void InitVulkan();
void CreateInstance();
void CreateWindowSurface();
void SetupDebugMessenger();
void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo);
void PickPhysicalDevice();
struct QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
bool IndicesIsComplete(struct QueueFamilyIndices indices);
bool IsDeviceSuitable(VkPhysicalDevice device);
void CreateLogicalDevice();
void MainLoop();
void Cleanup();
bool CheckValidationLayerSupport();
bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
void GetRequiredExtensions();
void QuerySwapChainSupport(VkPhysicalDevice device);
VkSurfaceFormatKHR ChooseSwapSurfaceFormat();
VkPresentModeKHR ChoosePresentMode();
VkExtent2D ChooseSwapExtent();
void CreateSwapChain();
void CreateImageViews();
void CreateRenderPass();
void CreateGraphicsPipeline();
VkShaderModule CreateShaderModule(const char* bufferCode, size_t bufferCodeLength);
void CreateFrameBuffers();
void CreateCommandPool();
void CreateCommandBuffers();
void CreateSyncObjects();
void DrawFrame();
void CleanupSwapChain();
void RecreateSwapchain();
void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void CreateTextureImage();
void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image
	, VkDeviceMemory* imageMemory);
VkCommandBuffer BeginSingleTimeCommands();
void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
void CreateTextureImageView();
VkImageView CreateImageView(VkImage image, VkFormat format);
void CreateTextureSampler();
void CheckVulkanResult(VkResult result, char* string);
char* ReadFile(const char* fileName);
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData);
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
void* maxNumber(void* a, void* b);
void* minNumber(void* a, void* b);
void UpdateFPS(double* previousTime, int* frameCount);