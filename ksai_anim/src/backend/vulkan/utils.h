#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "init.h"
#include <vulkan/vulkan.h>

struct queue_family_indices
{
	uint32_t graphics_family;
	uint32_t present_family;
};

struct swap_chain_support_details
{
	VkSurfaceCapabilitiesKHR capabilities;
	VkSurfaceFormatKHR formats[100];
	int formats_size;
	VkPresentModeKHR present_modes[100];
	int present_modes_size;
};

typedef struct queue_family_indices queue_family_indices;
typedef struct swap_chain_support_details swap_chain_support_details;

queue_family_indices find_queue_families_util(VkPhysicalDevice device);

VkShaderModule create_shader_module_util(char* code, int size);

int read_file_util(const char* file_name, char** string);

uint32_t find_memory_type_util(uint32_t type_filter, VkMemoryPropertyFlags properties);

void create_buffer_util(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* buffer_memory, VkDevice _vk_logical_device);

void create_image_util(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* image_memory, VkDevice _vk_logical_device);

void create_image_util_array(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *image, VkDeviceMemory *image_memory, VkDevice _vk_logical_device, uint32_t array_layers);

void create_image_util_array_cube(VkImageType image_type, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *image, VkDeviceMemory *image_memory, VkDevice _vk_logical_device, uint32_t array_layers);

VkCommandBuffer begin_single_time_commands_util(VkCommandPool _vk_command_pool);

void end_single_time_commands_util(VkCommandBuffer* command_buffer, VkQueue _vk_graphics_queue);

void transition_image_layout_util(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout, VkCommandPool _vk_command_pool, VkQueue _vk_graphics_queue);

void copy_buffer_to_image_util(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkCommandPool _vk_command_pool, VkQueue _vk_graphics_queue);

void copy_buffer_to_image_util_layered(uint32_t layer_count, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkCommandPool _vk_command_pool, VkQueue _vk_graphics_queue);

VkImageView create_image_view_util(VkImage image, VkFormat format);

VkImageView create_image_view_util2(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags);

VkImageView create_image_view_util2_skybox(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags);

void create_texture_sampler(VkSampler* _sampler, VkDevice _vk_logical_device, VkPhysicalDevice _vk_physical_device);

void create_texture_sampler_skybox(VkSampler *_sampler, VkDevice _vk_logical_device, VkPhysicalDevice _vk_physical_device);
