#include "utils.h"
#include "init.h"
#include <intrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

void create_image_util(
	uint32_t width,
	uint32_t height,
	VkFormat format,
	VkImageTiling tiling,
	VkImageUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkImage *image,
	VkDeviceMemory *image_memory,
	VkDevice _vk_logical_device
)
{
	VkImageCreateInfo image_info = { 0 };
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.extent.width = (uint32_t) width;
	image_info.extent.height = (uint32_t) height;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;
	image_info.arrayLayers = 1;
	// No mipmapping and no array
	image_info.format = format;
	image_info.tiling = tiling;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.usage = usage;
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_info.samples = VK_SAMPLE_COUNT_1_BIT;
	// related to multisamplinga
	image_info.flags = 0;

	if (vkCreateImage(_vk_logical_device, &image_info, NULL, image) != VK_SUCCESS)
	{
		printf("Failed to create an image \n");
	}

	VkMemoryRequirements mem_requirements;
	vkGetImageMemoryRequirements(_vk_logical_device, *image, &mem_requirements);

	VkMemoryAllocateInfo alloc_info = { 0 };
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = mem_requirements.size;
	alloc_info.memoryTypeIndex = find_memory_type_util(mem_requirements.memoryTypeBits, properties);

	if (vkAllocateMemory(_vk_logical_device, &alloc_info, NULL, image_memory) != VK_SUCCESS)
	{
		printf("Failed to allocate image memory");
	}

	vkBindImageMemory(_vk_logical_device, *image, *image_memory, 0);
}

VkCommandBuffer begin_single_time_commands_util(
	VkCommandPool _vk_command_pool
)
{
	VkCommandBufferAllocateInfo alloc_info = { 0 };
	alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	alloc_info.commandPool = _vk_command_pool;
	alloc_info.commandBufferCount = 1;

	VkCommandBuffer command_buffer;
	vkAllocateCommandBuffers(vk_logical_device_, &alloc_info, &command_buffer);

	VkCommandBufferBeginInfo begin_info = { 0 };
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(command_buffer, &begin_info);

	return command_buffer;
}

void end_single_time_commands_util(
	VkCommandBuffer *command_buffer,
	VkQueue _vk_graphics_queue
)
{
	vkEndCommandBuffer(*command_buffer);

	VkSubmitInfo submit_info = { 0 };
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = command_buffer;

	vkQueueSubmit(_vk_graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
}

void transition_image_layout_util(
	VkImage image,
	VkFormat format,
	VkImageLayout old_layout,
	VkImageLayout new_layout,
	VkCommandPool _vk_command_pool,
	VkQueue _vk_graphics_queue
)
{
	VkCommandBuffer command_buffer = begin_single_time_commands_util(_vk_command_pool);
	// for ensuring write to buffer completes before reading from it
	VkImageMemoryBarrier barrirer = { 0 };
	barrirer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrirer.oldLayout = old_layout;
	barrirer.newLayout = new_layout;
	barrirer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrirer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	// qeueu family ownership transfer gara ko laagi used hune ho
	barrirer.image = image;
	barrirer.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrirer.subresourceRange.baseMipLevel = 0;
	barrirer.subresourceRange.levelCount = 1;
	barrirer.subresourceRange.baseArrayLayer = 0;
	barrirer.subresourceRange.layerCount = 1;
	barrirer.srcAccessMask = 0;
	barrirer.dstAccessMask = 0;

	VkPipelineStageFlags source_stage;
	VkPipelineStageFlags destination_stage;

	if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrirer.srcAccessMask = 0;
		barrirer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrirer.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrirer.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		barrirer.srcAccessMask = 0;
		barrirer.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		source_stage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrirer.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		barrirer.srcAccessMask = 0;
		barrirer.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		old_layout = 0;
		new_layout = 0;
		source_stage = 0;
		destination_stage = 0;
		printf("Invalid argument");
		__debugbreak();
	}

	vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage,
		0,
		0,
		NULL,
		0,
		NULL,
		1,
		&barrirer
	);

	end_single_time_commands_util(&command_buffer, _vk_graphics_queue);
}

void copy_buffer_to_image_util(
	VkBuffer buffer,
	VkImage image,
	uint32_t width,
	uint32_t height,
	VkCommandPool _vk_command_pool,
	VkQueue _vk_graphics_queue
)
{
	VkCommandBuffer command_buffer = begin_single_time_commands_util(_vk_command_pool);
	VkBufferImageCopy region = { 0 };
	region = (VkBufferImageCopy){
		.bufferOffset = 0,
		.bufferRowLength = 0, // how the pixels are laid out in memory
		.bufferImageHeight = 0,
		.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.imageSubresource.mipLevel = 0,
		.imageSubresource.layerCount = 1,
		.imageOffset = {0, 0, 0},
		.imageExtent = {
			width, height, 1
		}
	};

	vkCmdCopyBufferToImage(
		command_buffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);


	end_single_time_commands_util(&command_buffer, _vk_graphics_queue);
}

VkImageView create_image_view_util(
	VkImage image,
	VkFormat format
)
{
	VkImageViewCreateInfo create_info = { 0 };
	create_info = (VkImageViewCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = VK_FORMAT_R8G8B8A8_SRGB,
		.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1,
		.components = {VK_COMPONENT_SWIZZLE_IDENTITY}
	};

	VkImageView image_view;
	if (vkCreateImageView(vk_logical_device_, &create_info, NULL, &image_view) != VK_SUCCESS)
	{
		printf("Failed to create image views \n");
	}
	return image_view;
}

VkImageView create_image_view_util2(
	VkImage image,
	VkFormat format,
	VkImageAspectFlags aspect_flags
)
{
	VkImageViewCreateInfo create_info = { 0 };
	create_info = (VkImageViewCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.subresourceRange.aspectMask = aspect_flags,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1,
		.components = {VK_COMPONENT_SWIZZLE_IDENTITY}
	};

	VkImageView image_view;
	if (vkCreateImageView(vk_logical_device_, &create_info, NULL, &image_view) != VK_SUCCESS)
	{
		printf("Failed to create image views \n");
	}
	return image_view;
}

// Not to be used
void create_texture_sampler(
	VkSampler *_sampler,
	VkDevice _vk_logical_device,
	VkPhysicalDevice _vk_physical_device
)
{
	VkPhysicalDeviceProperties properties = { 0 };
	vkGetPhysicalDeviceProperties(_vk_physical_device, &properties);

	VkSamplerCreateInfo sampler_info = { 0 };
	sampler_info = (VkSamplerCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.anisotropyEnable = VK_TRUE,
		.maxAnisotropy = properties.limits.maxSamplerAnisotropy,
		// for maximum quality
		.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		.unnormalizedCoordinates = VK_FALSE,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_ALWAYS,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.mipLodBias = 0.0f,
		.minLod = 0.0f,
		.maxLod = 0.0f
	};

	if (vkCreateSampler(vk_logical_device_, &sampler_info, NULL, _sampler) != VK_SUCCESS)
	{
		printf("Failed ot create texture sampler \n");
	}
}

queue_family_indices find_queue_families_util(VkPhysicalDevice device)
{
	queue_family_indices indcs = { 0 };
	uint32_t queue_family_count;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);

	VkQueueFamilyProperties queue_family_properties[100];
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_family_properties);

	if (queue_family_count == 0)
	{
		printf("No queue families supported\n");
	}

	for (int i = 0; i < queue_family_count; i++)
	{
		if ((queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && (queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT))
		{
			indcs.graphics_family = i;
		}

		/* JUST A WORK AROUND FOR NOW*/
		VkBool32 present_support = 1;
		if (vk_physical_device_ != NULL)
		{
			//vkGetPhysicalDeviceSurfaceSupportKHR(vk_physical_device_, i, vk_surface_, &present_support);
		}

		if (present_support)
		{
			indcs.present_family = i;
		}


	}

	return indcs;
}

VkShaderModule create_shader_module_util(
	char *code,
	int size
)
{
	VkShaderModuleCreateInfo create_info = { 0 };
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = size;
	create_info.pCode = (const uint32_t*) code;
	VkShaderModule shader_module;

	if (vkCreateShaderModule(vk_logical_device_, &create_info, NULL, &shader_module) != VK_SUCCESS)
	{
		printf("Failed to create shader modulee\n");
	}
	return shader_module;
}

int read_file_util(
	const char *file_name,
	char **string
)
{
	FILE *file = NULL;
	int size = 0;
	fopen_s(&file, file_name, "rb");
	if (file != NULL)
	{
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		fseek(file, 0, SEEK_SET);
		*string = (char*)malloc((size_t)size + 1);
		fread(*string, 1, size, file);
		fclose(file);
	}
	else {
		__debugbreak();
	}
	return size;
}

uint32_t find_memory_type_util(
	uint32_t type_filter,
	VkMemoryPropertyFlags properties
)
{
	VkPhysicalDeviceMemoryProperties mem_properties;
	/*
	typedef struct VkPhysicalDeviceMemoryProperties {
		uint32_t        memoryTypeCount;
		VkMemoryType    memoryTypes[VK_MAX_MEMORY_TYPES];
		uint32_t        memoryHeapCount;
		VkMemoryHeap    memoryHeaps[VK_MAX_MEMORY_HEAPS];
	} VkPhysicalDeviceMemoryProperties;
	*/
	vkGetPhysicalDeviceMemoryProperties(vk_physical_device_, &mem_properties);

	for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
	{
		if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) /* Yo k gareko ho hawa jpt */
		{
			return i;
		}
	}
	return 0;

}


void create_buffer_util(
	VkDeviceSize size,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties,
	VkBuffer *buffer,
	VkDeviceMemory *buffer_memory,
	VkDevice _vk_logical_device
)
{
	VkBufferCreateInfo create_info = { 0 };
	create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	create_info.size = size;
	create_info.usage = usage;
	create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	/* Since only used by graphics queue */

	if (vkCreateBuffer(_vk_logical_device, &create_info, NULL, buffer) != VK_SUCCESS)
	{
		printf("Failed to create vertex Buffers\n");
	}

	VkMemoryRequirements memory_requirements = { 0 };
	/*
	size = size of req mem in bytes, bufferInfo.size
	alignment = offset in bytes where the buffer begins in the allocated region of memory
	memoryTypeBits = bit field???? of memory types that are suitable for buffer 
	*/
	vkGetBufferMemoryRequirements(_vk_logical_device, *buffer, &memory_requirements);

	VkMemoryAllocateInfo alloc_info = { 0 };
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = memory_requirements.size;
	alloc_info.memoryTypeIndex = find_memory_type_util(memory_requirements.memoryTypeBits, properties);
	
	if (vkAllocateMemory(_vk_logical_device, &alloc_info, NULL, buffer_memory) != VK_SUCCESS)
	{
		printf("Failed to Allocate Vertex Buffer Memory\n");
	}
	// Allocate matrai haina, bind ni garnu paro buffer sanga, buffer ko memory chae yo ho hai vnna paron ta lul
	vkBindBufferMemory(_vk_logical_device, *buffer, *buffer_memory, 0);

}


