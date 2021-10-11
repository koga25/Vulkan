#pragma once
#include "vulkan/vulkan.h"
#include <stdbool.h>

VkCommandPoolCreateInfo CreateCommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
{
	VkCommandPoolCreateInfo info = { 0 };
	info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.pNext = NULL;

	info.flags = flags;
	return info;
}

VkCommandBufferAllocateInfo CreateCommandBufferAllocateInfo(VkCommandPool pool, uint32_t count, VkCommandBufferLevel level)
{
	VkCommandBufferAllocateInfo info = { 0 };
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.pNext = NULL;

	info.commandPool = pool;
	info.commandBufferCount = count;
	info.level = level;
	return info;
}

VkCommandBufferBeginInfo CreateCommandBufferBeginInfo(VkCommandBufferUsageFlags flags)
{
	VkCommandBufferBeginInfo info = { 0 };
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.pNext = NULL;

	info.pInheritanceInfo = NULL;
	info.flags = flags;
	return info;
}

VkFenceCreateInfo CreateFenceCreateInfo(VkFenceCreateFlags flags)
{
	VkFenceCreateInfo info = { 0 };
	info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	info.pNext = NULL;

	info.flags = flags;

	return info;
}

VkSemaphoreCreateInfo CreateSemaphoreCreateInfo(VkSemaphoreCreateFlags flags)
{
	VkSemaphoreCreateInfo info = { 0 };
	info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	info.pNext = NULL;
	info.flags = flags;
	return info;
}

VkPipelineShaderStageCreateInfo CreatePipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shaderModule)
{
	VkPipelineShaderStageCreateInfo info = { 0 };
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	info.pNext = NULL;
	info.stage = stage;
	info.module = shaderModule;
	info.pName = "main";

	return info;
}

VkPipelineVertexInputStateCreateInfo CreateVertexInputStateCreateInfo()
{
	VkPipelineVertexInputStateCreateInfo info = { 0 };
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	info.pNext = NULL;
	info.vertexBindingDescriptionCount = 0;
	info.vertexAttributeDescriptionCount = 0;

	return info;
}

VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyCreateInfo(VkPrimitiveTopology topology)
{
	VkPipelineInputAssemblyStateCreateInfo info = { 0 };
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	info.pNext = NULL;
	info.topology = topology;
	info.primitiveRestartEnable = VK_FALSE;

	return info;
}

VkPipelineRasterizationStateCreateInfo CreateRasterizationStateCreateInfo(VkPolygonMode polygonMode)
{
	VkPipelineRasterizationStateCreateInfo info = { 0 };
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	info.pNext = NULL;
	info.depthClampEnable = VK_FALSE;
	info.rasterizerDiscardEnable = VK_FALSE;
	info.polygonMode = polygonMode;
	info.lineWidth = 1.0f;
	info.cullMode = VK_CULL_MODE_NONE;
	info.frontFace = VK_FRONT_FACE_CLOCKWISE;
	info.depthBiasEnable = VK_FALSE;
	info.depthBiasConstantFactor = 0.0f;
	info.depthBiasClamp = 0.0f;
	info.depthBiasSlopeFactor = 0.0f;

	return info;
}

VkPipelineMultisampleStateCreateInfo CreateMultisamplingStateCreateInfo()
{
	VkPipelineMultisampleStateCreateInfo info = { 0 };
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	info.pNext = NULL;
	info.sampleShadingEnable = VK_FALSE;
	info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	info.minSampleShading = 1.0f;
	info.pSampleMask = NULL;
	info.alphaToCoverageEnable = VK_FALSE;
	info.alphaToOneEnable = VK_FALSE;

	return info;
}

VkPipelineColorBlendAttachmentState CreateColorBlendAttachmentState()
{
	VkPipelineColorBlendAttachmentState colorBlendAttachment = { 0 };
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

	return colorBlendAttachment;
}

VkPipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo()
{
	VkPipelineLayoutCreateInfo info = { 0 };
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	info.pNext = NULL;

	info.flags = 0;
	info.setLayoutCount = 0;
	info.pSetLayouts = 0;
	info.pushConstantRangeCount = 0;
	info.pPushConstantRanges = NULL;
	return info;
}

VkImageCreateInfo CreateImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent, uint32_t arrayLayers)
{
	VkImageCreateInfo info = { 0 };
	info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info.pNext = NULL;
	info.imageType = VK_IMAGE_TYPE_2D;
	info.format = format;
	info.extent = extent;
	info.mipLevels = 1;
	info.arrayLayers = arrayLayers;
	info.samples = VK_SAMPLE_COUNT_1_BIT;
	info.tiling = VK_IMAGE_TILING_OPTIMAL;
	info.usage = usageFlags;

	return info;
}

VkImageViewCreateInfo CreateImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags, VkImageViewType viewType, uint32_t layerCount)
{
	VkImageViewCreateInfo info = { 0 };
	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.pNext = NULL;
	info.viewType = viewType;
	info.components = (VkComponentMapping){ VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
	info.image = image;
	info.format = format;
	info.subresourceRange.baseMipLevel = 0;
	info.subresourceRange.levelCount = 1;
	info.subresourceRange.baseArrayLayer = 0;
	info.subresourceRange.layerCount = layerCount;
	info.subresourceRange.aspectMask = aspectFlags;

	return info;
}

VkPipelineDepthStencilStateCreateInfo CreateDepthStencilCreateInfo(bool depthTest, bool depthWrite, VkCompareOp compareOp)
{
	VkPipelineDepthStencilStateCreateInfo info = { 0 };
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	info.pNext = NULL;

	info.depthTestEnable = depthTest ? VK_TRUE : VK_FALSE;
	info.depthWriteEnable = depthWrite ? VK_TRUE : VK_FALSE;
	info.depthCompareOp = depthTest ? compareOp : VK_COMPARE_OP_ALWAYS;
	info.depthBoundsTestEnable = VK_FALSE;
	info.minDepthBounds = 0.0f; // Optional
	info.maxDepthBounds = 1.0f; // Optional
	info.stencilTestEnable = VK_FALSE;

	return info;
}

VkDescriptorSetLayoutBinding CreateDescriptorSetLayoutBinding(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding)
{
	VkDescriptorSetLayoutBinding setbind = { 0 };
	setbind.binding = binding;
	setbind.descriptorCount = 1;
	setbind.descriptorType = type;
	setbind.pImmutableSamplers = NULL;
	setbind.stageFlags = stageFlags;

	return setbind;
}

VkWriteDescriptorSet CreateWriteDescriptorBuffer(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorBufferInfo* bufferInfo, uint32_t binding)
{
	VkWriteDescriptorSet write = { 0 };
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = NULL;
	write.dstBinding = binding;
	write.dstSet = dstSet;
	write.descriptorCount = 1;
	write.descriptorType = type;
	write.pBufferInfo = bufferInfo;

	return write;
}


VkSubmitInfo CreateSubmitInfo(VkCommandBuffer* cmd)
{
	VkSubmitInfo info = { 0 };
	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	info.pNext = NULL;
	info.waitSemaphoreCount = 0;
	info.pWaitSemaphores = NULL;
	info.pWaitDstStageMask = NULL;
	info.commandBufferCount = 1;
	info.pCommandBuffers = cmd;
	info.signalSemaphoreCount = 0;
	info.pSignalSemaphores = NULL;

	return info;
}

VkSamplerCreateInfo CreatesamplerCreateInfo(VkFilter filters, VkSamplerAddressMode samplerAddressMode /*VK_SAMPLER_ADDRESS_MODE_REPEAT*/)
{
	VkSamplerCreateInfo info = { 0 };
	info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	info.pNext = NULL;
	info.magFilter = filters;
	info.minFilter = filters;
	info.addressModeU = samplerAddressMode;
	info.addressModeV = samplerAddressMode;
	info.addressModeW = samplerAddressMode;

	return info;
}

VkWriteDescriptorSet CreatewriteDescriptorImage(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorImageInfo* imageInfo, uint32_t binding)
{
	VkWriteDescriptorSet write = { 0 };
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.pNext = NULL;
	write.dstBinding = binding;
	write.dstSet = dstSet;
	write.descriptorCount = 1;
	write.descriptorType = type;
	write.pImageInfo = imageInfo;
	
	return write;
}

