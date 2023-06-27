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
#include <engine/renderer/scene.h>
#include "loaders/obj_loader.h"
#include "frontend/app_ui.h"
#include "frontend/3d_viewport.h"


int main(int argc, char *argv[])
{
	vk_rsrs resources;
	VkInstance instance;

	SDL_Init(SDL_INIT_EVERYTHING);
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
	kie_Camera viewport_camera;
	threeD_viewport_init(&viewport_camera);
	renderer_backend checker_renderer;
	initialize_renderer_backend(&resources, &checker_renderer);

	kie_Scene scene1;
	kie_Object obj1, obj2;
	kie_Object_init(&obj1);
	kie_Object_init(&obj2);

	read_obj_to_kie_Object("res/objs/sphere.obj", &obj1);
	read_obj_to_kie_Object("res/objs/plane.obj", &obj2);

	kie_Scene_init(&scene1);
	kie_Scene_add_object(&scene1, &obj1);
	kie_Scene_add_object(&scene1, &obj2);


	bool running = true;
	while (running)
	{
		SDL_Event windowEvent;
		SDL_WaitEventTimeout(&windowEvent, 400);

		copy_scene_to_backend(&resources, &scene1, &checker_renderer);
		static int width, height;
		SDL_GetWindowSize(resources.window, &width, &height);
		float aspect = (float)width/height;

		threeD_viewport_events(&viewport_camera, &scene1, &checker_renderer, resources.window, &windowEvent);

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

		static vec2 debug_window_pos = { 0.75, -0.7 }; static bool move_debug_window = false;
		draw_window("Debug    ", 7, debug_window_pos, aspect,  &resources, &windowEvent, &move_debug_window);
		char log[100];
		sprintf_s(log, sizeof(char) * 100, "cp:(%.2f,%.2f,%.2f)", viewport_camera.position[0], viewport_camera.position[1], viewport_camera.position[2]);
		draw_label_window(log, debug_window_pos, &resources, aspect, 1);
		sprintf_s(log, sizeof(char) * 100, "cr:(%.2f,%.2f,%.2f)", viewport_camera.rotation[0], viewport_camera.rotation[1], viewport_camera.rotation[2]);
		draw_label_window(log, debug_window_pos, &resources, aspect, 2);
		sprintf_s(log, sizeof(char) * 100, "Object Count:%d", scene1.objects_count);
		draw_label_window(log, debug_window_pos, &resources, aspect, 3);

		static vec2 demo_window_pos = { 0.75, -0.05 }; static bool move_demo_window = false;
		draw_window("Demo    ", 4, demo_window_pos, aspect,  &resources, &windowEvent, &move_demo_window);
		draw_label_window("Start", demo_window_pos, &resources, aspect, 1);
		draw_label_window("Clear Color", demo_window_pos, &resources, aspect, 1.5);
		char select[100][100] = {"blue", "lemon", "grey", "sprbud"};
		static vec3 colors[100];
		glm_vec3_copy(color_ALICEBLUE, colors[0]);
		glm_vec3_copy(color_LEMONCHIFFON, colors[1]);
		glm_vec3_copy(color_BATTLESHIPGREY, colors[2]);
		glm_vec3_copy(color_MEDIUMSPRINGBUD, colors[3]);
		static int selection = 0;
		draw_selector_window(select, 4, aspect, demo_window_pos, &resources, 2.5, &selection);

		if (windowEvent.type == SDL_QUIT)
		{
			running = false;
			break;
		}

		{
			ui_events(resources.window, &windowEvent);
		}




		if (draw_backend_start(&resources) != -1 && running)
		{
			threeD_viewport_update(&viewport_camera, &scene1, &checker_renderer, resources.window, &windowEvent, &resources);
			ui_update(&resources.current_frame);
			draw_backend_begin(&resources, colors[selection]);
			
			threeD_viewport_draw(&viewport_camera, &scene1, &checker_renderer, &resources);
			ui_render(&resources.current_frame, &resources);

			draw_backend_end(&resources);
			draw_backend_finish(&resources);
		}
		else
		{
			draw_backend_wait(&resources);
		}

	}

	destroy_renderer_backend(&resources, &checker_renderer);
	ui_destroy(&resources);
	destroy_backend(&resources);

	SDL_DestroyWindow(resources.window);
	SDL_Quit();
	return 0;
}
