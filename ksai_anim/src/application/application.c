#if defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdbool.h>
#include <ksai/ksai_primitives.h>
#include <backend/vulkan/backend.h>
#include <backend/ui/ui.h>
#include <backend/ui/latex_colors.h>
#include "frontend/app_ui.h"


int main(int argc, char *argv[])
{
	vk_rsrs resources;
	VkInstance instance;

	SDL_Init(SDL_INIT_VIDEO);
	resources.window  = SDL_CreateWindow(
		"NAME",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		640,
		360,
		SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE
	);

	initialize_backend(&resources, &instance);
	lu_int(10, &resources);


	bool running = true;
	int i = 1;
	int width, height;
	while (running)
	{
		SDL_GetWindowSize(resources.window, &width, &height);
		SDL_Event windowEvent;


		char m[6][10]= {0};
		int c[6] = {0};
		draw_file_menu(m, 4,  c, (float)width/height, &resources, &windowEvent);



		while (SDL_PollEvent(&windowEvent))
		{
			if (windowEvent.type == SDL_QUIT)
			{
				running = false;
				break;
			}
		}

		if(draw_backend_start(&resources)!=-1)
		{ 
			lu_updt(&resources.current_frame);
			draw_backend_begin(&resources);

			lu_rndr(&resources.current_frame, &resources);

			draw_backend_end(&resources);
			draw_backend_finish(&resources);
		}
		else
		{
			draw_backend_wait(&resources);
		}

	}

	SDL_DestroyWindow(resources.window);
	SDL_Quit();
	return 0;
}
