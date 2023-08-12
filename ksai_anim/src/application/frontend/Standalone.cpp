#include "vulkan/vulkan.hpp"
#include "PostProcessing.hpp"

extern "C"
{
#include <backend/vulkan/backend.h>
#include "3d_viewport.h"
#include <SDL2/SDL_vulkan.h>

	static SDL_Window* PresentWindow;
	static VkSurfaceKHR Surface;

	void CreateExternalWindow(vk_rsrs* rsrs, VkInstance inInstance)
	{
		vk::Instance I = inInstance;
		PresentWindow = SDL_CreateWindow(
			"PP",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			rsrs->vk_swap_chain_image_extent_2d_.width,
			rsrs->vk_swap_chain_image_extent_2d_.height,
			SDL_WINDOW_HIDDEN | SDL_WINDOW_VULKAN
		);
		if (SDL_Vulkan_CreateSurface(PresentWindow, I, &Surface) == SDL_FALSE);
	}

	void ShowExternalWindow()
	{
		SDL_ShowWindow(PresentWindow);
		SDL_MaximizeWindow(PresentWindow);
	}

	void HideExternalWindow()
	{
		SDL_HideWindow(PresentWindow);
	}

	void PresentToExternalWindow(kie_Camera* camera, kie_Scene* scene, uint32_t viewport_obj_count, vk_rsrs* rsrs, renderer_backend* backend, int* inCurrentPipeline)
	{
		SDL_Event e;
		while (true)
		{
			SDL_WaitEvent(&e);
			if (e.window.event == SDL_WINDOWEVENT_CLOSE)
			{
				HideExternalWindow();
				return;
			}
		}
	}

	void DestroyExternalWindow(VkInstance inInstance)
	{
		//vk::Device D = vk_logical_device_;
		vk::Instance I = inInstance;
		I.destroySurfaceKHR(Surface);
		SDL_DestroyWindow(PresentWindow);
	}
}