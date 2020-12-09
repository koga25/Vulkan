#pragma once

#include <stdio.h>
#include <stdbool.h>

#include "Vendors/Nuklear/Nuklear.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#ifndef NK_GLFW_TEXT_MAX
#define NK_GLFW_TEXT_MAX 256
#endif
#ifndef NK_GLFW_DOUBLE_CLICK_LO
#define NK_GLFW_DOUBLE_CLICK_LO 0.02
#endif
#ifndef NK_GLFW_DOUBLE_CLICK_HI
#define NK_GLFW_DOUBLE_CLICK_HI 0.2
#endif

#define VK_COLOR_COMPONENT_MASK_RGBA VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_INDEX_BUFFER 128 * 1024

struct nkVulkanContext
{
	struct nk_buffer buffer;
	struct nk_draw_null_texture nullTexture;
	struct NuklearQueueFamilyIndices
	{
		uint32_t graphicsFamily;
		bool graphicsFamilyFound;

		//queue family that has the capability of presenting to window surface.
		uint32_t presentFamily;
		bool presentFamilyFound;
	}nuklearFamilyIndices;

	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties physicalDeviceProperties;
	VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

	VkDevice logicalDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	struct NuklearSwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		VkSurfaceFormatKHR* formats;
		uint32_t formatsLength;
		VkPresentModeKHR* presentModes;
		uint32_t presentModesLength;
	}nuklearSwapChainDetails;

	VkSwapchainKHR swapChain;
	VkImage* swapChainImages;
	uint32_t swapChainImagesCount;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	VkImageView* swapChainImageViews;

	struct NuklearVertFragBufferDetails
	{
		size_t vertexShaderBufferLength;
		size_t fragmentShaderBufferLength;
	}nuklearVertFragBufferDetails;

	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	VkFramebuffer* swapChainFrameBuffers;

	VkCommandPool commandPool;
	VkCommandBuffer* commandBuffers;

	VkSemaphore renderCompleted;
	size_t currentFrame;

	bool framebufferResized;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	VkBuffer uniformBuffers;
	VkDeviceMemory uniformBuffersMemory;

	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSets;

	VkImage fontImage;
	VkDeviceMemory textureImageMemory;
	VkImageView fontImageView;
	VkSampler fontSampler;
}nkVulkan;

static struct nkUniformBufferObject
{
	mat4 proj;
}nkUbo;

struct Mat4f
{
	float m[16];
};

struct nkVertex
{
	vec2 pos[2];
	vec2 uv[2];
	nk_byte col[4];
};

struct overlaySettings
{
	float bg_color[4];
	uint8_t orientation;
	int zoom;
}nkSettings;

extern struct VertexHelper nkVertexHelper;
extern struct nk_context* nkContext; //not to be confused with context from Vulkan.c or nkVulkan from this file
extern struct nk_color background;
extern struct nk_font_atlas* atlas;
extern struct nk_vec2 scroll;
extern unsigned int text[NK_GLFW_TEXT_MAX];
extern int textLength;
extern double lastButtonClick;
extern int isDoubleClick;
extern struct nk_vec2 doubleClickPosition;
extern int windowWidth;
extern int windowHeight;
extern int displayWidth;
extern int displayHeight;
extern struct nk_vec2 fbScale;
extern struct nk_buffer nkCmd;

extern bool doWeRotate;


enum nkGlfwInitState {
	NK_GLFW3_DEFAULT = 0,
	NK_GLFW3_INSTALL_CALLBACKS
};

enum {UP, DOWN};

NK_API void nk_glfw3_shutdown();
NK_API void nk_glfw3_font_stash_begin(struct nk_font_atlas* atlas);
NK_API void nk_glfw3_font_stash_end();
NK_API void nk_glfw3_new_frame();
NK_API VkSemaphore nk_glfw3_render(enum nk_anti_aliasing AA, uint32_t bufferIndex, VkSemaphore waitSemaphore);

NK_API void nk_glfw3_device_destroy();
NK_API void nk_glfw3_device_create(VkDevice, VkPhysicalDevice, VkQueue graphics_queue, uint32_t graphics_queue_index, VkFramebuffer* framebuffers, uint32_t framebuffers_len, VkFormat, VkFormat);

NK_API void nk_glfw3_char_callback(GLFWwindow* win, unsigned int codepoint);
NK_API void nk_gflw3_scroll_callback(GLFWwindow* win, double xoff, double yoff);
NK_API void nk_glfw3_mouse_button_callback(GLFWwindow* win, int button, int action, int mods);

NK_INTERN void nk_glfw3_clipboard_copy(nk_handle user, const char* text, int length);
NK_INTERN void nk_glfw3_clipboard_paste(nk_handle user, struct nk_text_edit* edit);

void InitNuklear();
void NuklearInstallCallbacks();
void NuklearGetContext();
void CopyDuplicates();
void NuklearCreateDescriptorSetLayout();
void NuklearCreateDescriptorPool();
void NuklearCreateDescriptorSets();
static VkVertexInputBindingDescription NuklearGetBindingDescription();	
static VkVertexInputAttributeDescription* NuklearGetAttributeDescriptions();
void NuklearCreateCommandPool();
void NuklearCreateCommandBuffers();
void NuklearCreateSyncObjects();
void NuklearCreateRenderPass();
void NuklearCreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);
void NuklearCreateGraphicsPipeline();
void NuklearCreateTextureImage(const void* image, int width, int height);
void NuklearUpdateDescriptorSets();
VkSemaphore SubmitOverlay(struct overlaySettings* settings, uint32_t bufferIndex, VkSemaphore mainFinishedSemaphore);
void CleanupNuklear();


