#include "init.h"
#include "sync.h"
#include "run.h"
#include "resize.h"
#include "cleanup.h"
#include <ksai/ksai_memory.h>
#include "backend.h"

static bool first_call = true;

void initialize_backend(vk_rsrs *rsrs, VkInstance *instance)
{
	rsrs->current_frame = 0;
	if (first_call == true)
	{
		create_vulkan_instace(rsrs->window, instance);
	}
	first_call = false;
	create_window_surface(rsrs->window, rsrs, true, *instance);
	pick_physical_and_logical_devices(rsrs, *instance);
	create_vulkan_render_pass(rsrs);
	vulkan_sync_init(rsrs);
}


int draw_backend_start(vk_rsrs *_rsrs)
{
	vkWaitForFences(vk_logical_device_, 1, &_rsrs->vk_inflight_fences_[_rsrs->current_frame], VK_TRUE, KSAI_U64_MAX);


	/* Acquiring an image from the swap chain */
	{
		_rsrs->result_next_image = vkAcquireNextImageKHR(vk_logical_device_, _rsrs->vk_swap_chain_, KSAI_U64_MAX, _rsrs->vk_image_available_semaphores_[_rsrs->current_frame], VK_NULL_HANDLE, &_rsrs->image_index);

		if (_rsrs->result_next_image == VK_ERROR_OUT_OF_DATE_KHR || _rsrs->result_next_image == VK_SUBOPTIMAL_KHR || frame_buffer_resized_)
		{
			frame_buffer_resized_ = 0;
			recreate_swap_chain(_rsrs);
			return -1;
		}
		else if (_rsrs->result_next_image != VK_SUCCESS && _rsrs->result_next_image != VK_SUBOPTIMAL_KHR)
		{
			printf("Failed to Aquire Swap Chain image");
			return -1;
		}
	}

	vkResetFences(
		vk_logical_device_,
		1,
		&_rsrs->vk_inflight_fences_[_rsrs->current_frame]
	);

	/* Recording the command buffer */

	vkDeviceWaitIdle(vk_logical_device_);
	vkResetCommandBuffer(vk_command_buffer_[_rsrs->current_frame], 0);
	VkCommandBufferBeginInfo begin_info = { 0 };
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = 0;
	begin_info.pInheritanceInfo = NULL;

	if (vkBeginCommandBuffer(vk_command_buffer_[_rsrs->current_frame], &begin_info) != VK_SUCCESS)
	{
		printf("FAILED to begin recording command buffer\n");
	}
}

int draw_backend_begin(vk_rsrs *_rsrs)
{

	VkRenderPassBeginInfo render_pass_info = { 0 };

	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.renderPass = _rsrs->vk_render_pass_;
	render_pass_info.framebuffer = _rsrs->vk_swap_chain_frame_buffers_[_rsrs->image_index];
	render_pass_info.renderArea.offset = (VkOffset2D){ 0, 0 };
	render_pass_info.renderArea.extent = _rsrs->vk_swap_chain_image_extent_2d_;


	VkClearValue clear_color[2] = {
		(VkClearValue)
		 {
		.color.float32[0] = 0,
		.color.float32[1] = 1,
		.color.float32[2] = 0,
		},

		(VkClearValue)
		{
		.depthStencil = {1.0f, 0.0f}
		}
	};
	render_pass_info.clearValueCount = 2;
	render_pass_info.pClearValues = clear_color;
	vkCmdSetDepthTestEnable(vk_command_buffer_[_rsrs->current_frame], VK_FALSE);
	vkCmdBeginRenderPass(vk_command_buffer_[_rsrs->current_frame], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
	return _rsrs->current_frame;
}

void draw_backend_end(vk_rsrs *_rsrs)
{
	vkCmdEndRenderPass(vk_command_buffer_[_rsrs->current_frame]);

	if (vkEndCommandBuffer(vk_command_buffer_[_rsrs->current_frame]) != VK_SUCCESS)
	{
		printf("Failed to record command buffer!");
	}

}

void draw_backend_finish(vk_rsrs *_rsrs)
{
	VkSemaphore signal_semaphores[] = { _rsrs->vk_render_finished_semaphore_[_rsrs->current_frame] };
	submit_the_command_buffer(&_rsrs->current_frame, (int *) &_rsrs->image_index, &_rsrs->result_next_image, _rsrs);

	present_to_queue(signal_semaphores, &_rsrs->image_index, &_rsrs->result_next_image, &_rsrs->current_frame, _rsrs);

	vkQueueWaitIdle(_rsrs->vk_graphics_queue_);
}

void draw_backend_wait(vk_rsrs *_rsrs)
{
	vkQueueWaitIdle(_rsrs->vk_graphics_queue_);
}
