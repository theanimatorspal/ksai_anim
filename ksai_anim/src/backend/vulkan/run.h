#pragma once
#include "init.h"

void present_to_queue(VkSemaphore *signal_semaphores, uint32_t *image_index, VkResult *result_next_image, int *current_frame_xt, vk_rsrs *_rsrs);

void submit_the_command_buffer(int *current_frame_xt, int *image_index, VkResult *result_next_image, vk_rsrs *_rsrs);

void run_main(vk_rsrs *_rsrs);
