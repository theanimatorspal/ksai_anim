#include "init.h"
#include "resize.h"
#include "run.h"

extern struct pipeline_vk _pipline_test_temporary;
struct pipeline_vk *pplns_[MAX_BUFFER_SIZE];
uint32_t vlkn_ppln_cnt = 0;

void 
present_to_queue(
	VkSemaphore *signal_semaphores,
	uint32_t *image_index,
	VkResult *result_next_image,
	int *current_frame_xt,
	vk_rsrs *_rsrs
)
{
	VkPresentInfoKHR present_info = { 0 };
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores = signal_semaphores;

	VkSwapchainKHR swap_chains[] = { _rsrs->vk_swap_chain_ };
	present_info.swapchainCount = 1;
	present_info.pSwapchains = swap_chains;
	present_info.pImageIndices = image_index;
	present_info.pResults = NULL;

	*result_next_image = vkQueuePresentKHR(_rsrs->vk_graphics_queue_, &present_info);
	/* Check */
	{
		if (*result_next_image == VK_ERROR_OUT_OF_DATE_KHR || *result_next_image == VK_SUBOPTIMAL_KHR)
		{
			recreate_swap_chain(_rsrs);
			return;
		}
		else if (*result_next_image != VK_SUCCESS && *result_next_image != VK_SUBOPTIMAL_KHR)
		{
			printf("Failed to Aquire Swap Chain image");
		}
	}
	*current_frame_xt = (*current_frame_xt + 1) % MAX_FRAMES_IN_FLIGHT;

}

void submit_the_command_buffer(
	int *current_frame_xt,
	int *image_index,
	VkResult *result_next_image,
	vk_rsrs *_rsrs
)
{
	/* Submitting the Command Buffer*/
	{
		VkSubmitInfo submit_info = { 0 };
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore wait_semaphores[] = { _rsrs->vk_image_available_semaphores_[*current_frame_xt] };
		VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = wait_semaphores;
		submit_info.pWaitDstStageMask = wait_stages;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &vk_command_buffer_[*current_frame_xt];

		VkSemaphore signal_semaphores[] = { _rsrs->vk_render_finished_semaphore_[*current_frame_xt] };
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = signal_semaphores;

		if (vkQueueSubmit(_rsrs->vk_graphics_queue_, 1, &submit_info, _rsrs->vk_inflight_fences_[*current_frame_xt]) != VK_SUCCESS)
		{
			printf("Failed to draw command buffer");
		}

		/* Presenting to the Queue */
	}
}

void run_main(vk_rsrs *_rsrs)
{
	static int current_frame_xt = 0;
	vkWaitForFences( vk_logical_device_, 1, &_rsrs->vk_inflight_fences_[current_frame_xt], VK_TRUE, UINT64_MAX);


	/* Acquiring an image from the swap chain */
	uint32_t image_index;
	VkResult result_next_image;
	{
		result_next_image = vkAcquireNextImageKHR(vk_logical_device_, _rsrs->vk_swap_chain_, UINT64_MAX, _rsrs->vk_image_available_semaphores_[current_frame_xt], VK_NULL_HANDLE, &image_index);

		if (result_next_image == VK_ERROR_OUT_OF_DATE_KHR || result_next_image == VK_SUBOPTIMAL_KHR || frame_buffer_resized_)
		{
			frame_buffer_resized_ = 0;
			recreate_swap_chain(_rsrs);
			return;
		}
		else if (result_next_image != VK_SUCCESS && result_next_image != VK_SUBOPTIMAL_KHR)
		{
			printf("Failed to Aquire Swap Chain image");
		}
	}

	vkResetFences(
		vk_logical_device_,
		1,
		&_rsrs->vk_inflight_fences_[current_frame_xt]
	);

	/* Recording the command buffer */

	vkDeviceWaitIdle(vk_logical_device_);
	vkResetCommandBuffer(vk_command_buffer_[current_frame_xt], 0);
	VkCommandBufferBeginInfo begin_info = { 0 };
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = 0;
	begin_info.pInheritanceInfo = NULL;

	if (vkBeginCommandBuffer(vk_command_buffer_[current_frame_xt], &begin_info) != VK_SUCCESS)
	{
		printf("FAILED to begin recording command buffer\n");
	}

	VkRenderPassBeginInfo render_pass_info = { 0 };

	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.renderPass = _rsrs->vk_render_pass_;
	render_pass_info.framebuffer = _rsrs->vk_swap_chain_frame_buffers_[image_index];
	render_pass_info.renderArea.offset = (VkOffset2D){ 0, 0 };
	render_pass_info.renderArea.extent = _rsrs->vk_swap_chain_image_extent_2d_;

		
	VkClearValue clear_color[2] = { 
		(VkClearValue) {
			.color.float32[0] = 0,
			.color.float32[1] = 1,
			.color.float32[2] = 0,
		},

		(VkClearValue) {
			.depthStencil = {1.0f, 0.0f}
		}
	};
	render_pass_info.clearValueCount = 2;
	render_pass_info.pClearValues = clear_color;
	vkCmdSetDepthTestEnable(vk_command_buffer_[current_frame_xt], VK_FALSE);


	vkCmdBeginRenderPass(vk_command_buffer_[current_frame_xt], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdEndRenderPass(vk_command_buffer_[current_frame_xt]);

	if (vkEndCommandBuffer(vk_command_buffer_[current_frame_xt]) != VK_SUCCESS)
	{
		printf("Failed to record command buffer!");
	}

	//VkSemaphore wait_semaphores[] = { _rsrs->vk_image_available_semaphores_[current_frame_xt] };
	VkSemaphore signal_semaphores[] = { _rsrs->vk_render_finished_semaphore_[current_frame_xt] };
	submit_the_command_buffer((uint32_t*) &current_frame_xt, (int*) & image_index, &result_next_image, _rsrs);

	present_to_queue(signal_semaphores, &image_index, &result_next_image, (uint32_t *) & current_frame_xt, _rsrs);

	vkQueueWaitIdle(_rsrs->vk_graphics_queue_);
}
