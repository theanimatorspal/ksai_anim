#include "init.h"

void vulkan_cleanup(vk_rsrs *_rsrs)
{
	vkDeviceWaitIdle(vk_logical_device_);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroyFramebuffer(vk_logical_device_, _rsrs->vk_swap_chain_frame_buffers_[i], NULL);
	}


	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(vk_logical_device_, _rsrs->vk_image_available_semaphores_[i], NULL);
		vkDestroySemaphore(vk_logical_device_, _rsrs->vk_render_finished_semaphore_[i], NULL);
		vkDestroyFence(vk_logical_device_, _rsrs->vk_inflight_fences_[i], NULL);
	}



	vkDestroyRenderPass(vk_logical_device_, _rsrs->vk_render_pass_, NULL);
	for (int i = 0; i < _rsrs->no_of_swap_chain_images_; i++)
	{
		vkDestroyImageView(vk_logical_device_, _rsrs->vk_swap_chain_image_views_[i], NULL);
	}
	vkDestroyImage(vk_logical_device_, _rsrs->vk_depth_image, NULL);
	vkDestroyImageView(vk_logical_device_, _rsrs->vk_depth_image_view, NULL);
	vkFreeMemory(vk_logical_device_, _rsrs->vk_depth_image_memory, NULL);
	vkDestroySwapchainKHR(vk_logical_device_, _rsrs->vk_swap_chain_, NULL);


	// DEVICE QUEUES ARE IMPLICITLY DESTROYED
	
	//vkDestroySurfaceKHR(vk_instance_, _rsrs->vk_srf, NULL);
	vkDestroyCommandPool(vk_logical_device_, vk_command_pool_, NULL);
	vkDestroyDevice(vk_logical_device_, NULL);
	vkDestroyInstance(vk_instance_, NULL);

}

void cleanup_swap_chain(vk_rsrs *_rsrs)
{
	vkDeviceWaitIdle(vk_logical_device_);
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(vk_logical_device_, _rsrs->vk_image_available_semaphores_[i], NULL);
		vkDestroySemaphore(vk_logical_device_, _rsrs->vk_render_finished_semaphore_[i], NULL);
		vkDestroyFence(vk_logical_device_, _rsrs->vk_inflight_fences_[i], NULL);
	}

	for (size_t i = 0; i < _rsrs->no_of_swap_chain_images_; i++)
	{
		vkDestroyFramebuffer(vk_logical_device_, _rsrs->vk_swap_chain_frame_buffers_[i], NULL);
	}

	for (size_t i = 0; i < _rsrs->no_of_swap_chain_images_; i++)
	{
		vkDestroyImageView(vk_logical_device_, _rsrs->vk_swap_chain_image_views_[i], NULL);
	}

	vkDestroySwapchainKHR(vk_logical_device_, _rsrs->vk_swap_chain_, NULL);
	vkDestroyImage(vk_logical_device_, _rsrs->vk_depth_image, NULL);
	vkDestroyImageView(vk_logical_device_, _rsrs->vk_depth_image_view, NULL);
	vkFreeMemory(vk_logical_device_, _rsrs->vk_depth_image_memory, NULL);
}
