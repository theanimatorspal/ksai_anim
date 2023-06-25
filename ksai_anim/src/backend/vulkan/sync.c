#include "init.h"
#include "sync.h"


void vulkan_sync_init(vk_rsrs *_rsrs)
{
	VkSemaphoreCreateInfo create_semaphore_info = { 0 };
	create_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo create_fence_info = { 0 };
	create_fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	create_fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	// Suru ma chae fence lai signal gardine


	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (
			vkCreateSemaphore(vk_logical_device_, &create_semaphore_info, NULL, &_rsrs->vk_image_available_semaphores_[i]) != VK_SUCCESS
			|| vkCreateSemaphore(vk_logical_device_, &create_semaphore_info, NULL, &_rsrs->vk_render_finished_semaphore_[i]) != VK_SUCCESS ||
			vkCreateFence(vk_logical_device_, &create_fence_info, NULL, &_rsrs->vk_inflight_fences_[i]
		   )
		)
		{
			printf("Failed to create semaphores:\n");
		}
	}
}
