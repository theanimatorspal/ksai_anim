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
	resources.window = SDL_CreateWindow(
		"NAME",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		1920 * 0.7,
		1080 * 0.7,
		SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE
	);

	initialize_backend(&resources, &instance);
	ui_init(100, &resources);


	bool running = true;
	while (running)
	{
		static int width, height;
		SDL_GetWindowSize(resources.window, &width, &height);
		float aspect = (float)width/height;
		SDL_Event windowEvent;
		//SDL_WaitEvent(&windowEvent);

		char m[6][10][100] = { 0 };
		int c[6] = { 3, 6, 0, 0 };
		strcpy_s(m[0][0], sizeof(char) * 100, "File");
		strcpy_s(m[1][0], sizeof(char) * 100, "Create");
		strcpy_s(m[2][0], sizeof(char) * 100, "Window");
		strcpy_s(m[3][0], sizeof(char) * 100, "Mesh ");
		strcpy_s(m[4][0], sizeof(char) * 100, "Render");

		strcpy_s(m[0][1], sizeof(char) * 100, "New");
		strcpy_s(m[0][2], sizeof(char) * 100, "Open");
		strcpy_s(m[0][3], sizeof(char) * 100, "Close");

		strcpy_s(m[1][1], sizeof(char) * 100, "Circle");
		strcpy_s(m[1][2], sizeof(char) * 100, "Plane");
		strcpy_s(m[1][3], sizeof(char) * 100, "Annulus");
		strcpy_s(m[1][4], sizeof(char) * 100, "Mesh");
		strcpy_s(m[1][5], sizeof(char) * 100, "Part");
		strcpy_s(m[1][6], sizeof(char) * 100, "Curv");
		draw_file_menu(m, 5, c, aspect, &resources, (int *) &running);

		static vec2 debug_window_pos = { 0, 0 }; static bool move_debug_window = false;
		draw_window("Debug    ", 4, debug_window_pos, aspect,  &resources, &windowEvent, &move_debug_window);
		draw_label_window("Start", debug_window_pos, &resources, aspect, 1);
		draw_label_window("Fuck You", debug_window_pos, &resources, aspect, 1.5);

		static vec2 demo_window_pos = { -0.25, 0.25 }; static bool move_demo_window = false;
		draw_window("Demo    ", 4, demo_window_pos, aspect,  &resources, &windowEvent, &move_demo_window);
		draw_label_window("Start", demo_window_pos, &resources, aspect, 1);
		draw_label_window("Fuck You", demo_window_pos, &resources, aspect, 1.5);
		char select[100][100] = {"Hello", "Hi", "Therefore", "Fuck_You"};
		static int selection = 0;
		draw_selector_window(select, 4, aspect, demo_window_pos, &resources, 2.5, &selection);


		SDL_WaitEvent(&windowEvent);
		{
			ui_events(resources.window, &windowEvent);
			if (windowEvent.type == SDL_QUIT)
			{
				running = false;
				break;
			}
		}


		if (draw_backend_start(&resources) != -1)
		{
			ui_update(&resources.current_frame);
			draw_backend_begin(&resources);

			ui_render(&resources.current_frame, &resources);

			draw_backend_end(&resources);
			draw_backend_finish(&resources);
		}
		else
		{
			draw_backend_wait(&resources);
		}

	}

	ui_destroy(&resources);
	destroy_backend(&resources);

	SDL_DestroyWindow(resources.window);
	SDL_Quit();
	return 0;
}
