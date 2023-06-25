#include "init.h"
#include "cleanup.h"
#include "resize.h"
#include "sync.h"
#include <stdbool.h>
#include <SDL2/SDL_vulkan.h>

bool frame_buffer_resized_ = false;


void recreate_swap_chain(vk_rsrs *_rsrs)
{
	int width, height;
	SDL_Vulkan_GetDrawableSize(_rsrs->window, &width, &height);
	while (width == 0 || height == 0)
	{
		SDL_Vulkan_GetDrawableSize(_rsrs->window, &width, &height);
		SDL_PumpEvents();
	}

	vkDeviceWaitIdle(vk_logical_device_);
	cleanup_swap_chain(_rsrs);

	create_swap_chain_image_views_render_pass(_rsrs);

	vulkan_sync_init(_rsrs);
}
