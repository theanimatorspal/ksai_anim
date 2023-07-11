#pragma once
#if defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <SDL2/SDL.h>
#include "utils.h"
#include <ksai/ksai.h>
#include <stdbool.h>

extern VkInstance vk_instance_;
extern VkPhysicalDevice vk_physical_device_;
extern VkDevice vk_logical_device_;
extern VkCommandPool vk_command_pool_;
extern VkCommandBuffer vk_command_buffer_[MAX_FRAMES_IN_FLIGHT];

/* Vulkan Resources Per window */
typedef struct vk_rsrs
{
	SDL_Window *window;
	VkSurfaceKHR vk_srf;
	VkQueue vk_graphics_queue_;
	VkQueue vk_present_queue_;
	VkQueue vk_compute_queue_;
	VkSemaphore vk_image_available_semaphores_[MAX_FRAMES_IN_FLIGHT];
	VkSemaphore vk_render_finished_semaphore_[MAX_FRAMES_IN_FLIGHT];
	VkFence vk_inflight_fences_[MAX_FRAMES_IN_FLIGHT];
	VkSwapchainKHR vk_swap_chain_;
	uint32_t no_of_swap_chain_images_;
	VkImage vk_swap_chain_images_[MAX_FRAMES_IN_FLIGHT];
	VkFormat vk_swap_chain_image_format_;
	VkExtent2D vk_swap_chain_image_extent_2d_;
	VkImageView vk_swap_chain_image_views_[MAX_FRAMES_IN_FLIGHT];
	VkFramebuffer vk_swap_chain_frame_buffers_[MAX_FRAMES_IN_FLIGHT];
	VkRenderPass vk_render_pass_;
	VkImage vk_depth_image;
	VkDeviceMemory vk_depth_image_memory;
	VkImageView vk_depth_image_view;
	VkFormat vk_depth_image_format;

	VkCommandPool mRenderCommandPool;

	bool frame_buffer_resized_;
	uint32_t image_index;
	VkResult result_next_image;
	int current_frame;
} vk_rsrs;


void create_window_surface(SDL_Window *_window, vk_rsrs *_rsrs, bool first_time, VkInstance instance);

void create_vulkan_render_pass(vk_rsrs *_rsrs);

void pick_physical_and_logical_devices(vk_rsrs *_rsrs, VkInstance instance);

void create_vulkan_instace(SDL_Window *window, VkInstance *instance);

void create_swap_chain_image_views_render_pass(vk_rsrs* _rsrs);