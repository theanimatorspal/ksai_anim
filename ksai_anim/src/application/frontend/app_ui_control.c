#include "app_ui.h"

void handle_file_menu(ivec2s file_option, float aspect, vk_rsrs *rsrs, SDL_Event *event, kie_Scene* scene, renderer_backend *backend)
{
	static ivec2s opt;
	static vec2 pos = {-0.75, 0.25};
	static bool move = false;
	static bool clicked = false;


	if (clicked == false)
	{
		opt = file_option;
	}
	switch (opt.x)
	{
	case 0:
		break;
	case 1:
		switch (opt.y)
		{
		case 0:
			clicked = true;
			static int selection = 8;
			draw_window("Circle", 5, pos, aspect, rsrs, event, &move);
			draw_selector_integer(4, 64, aspect, pos, rsrs, 2, &selection);
			draw_label_window("Divisions:", pos, rsrs, aspect, 1);
			if (draw_button_window("Create", pos, rsrs, aspect, 3))
			{
				kie_Object object;
				kie_Object_init(&object);
				kie_Object_create_circle(
					&object,
					3,
					selection,
					(vec3) {0, 0, 0},
					(vec3) {0, 0, 0},
					true,
					0,
					0
				);
				kie_Scene_add_object(scene,1, &object);
				copy_scene_to_backend(rsrs, scene, backend);
				clicked = false;
			}
			break;
		case 1:
			clicked = true;
			static int divs = 8;
			static int height = 8;
			draw_window("Cylinder", 6, pos, aspect, rsrs, event, &move);
			draw_label_window("Divisions:", pos, rsrs, aspect, 1);
			draw_selector_integer(4, 64, aspect, pos, rsrs, 2, &divs);
			draw_label_window("height", pos, rsrs, aspect, 3);
			draw_selector_integer(1, 50, aspect, pos, rsrs, 4, &height);

			if (draw_button_window("Create", pos, rsrs, aspect, 5))
			{
				kie_Object object;
				kie_Object_init(&object);
				kie_Object_create_cylinder(&object, 3, divs, (float)height);
				kie_Scene_add_object(scene,1, &object);
				copy_scene_to_backend(rsrs, scene, backend);
				clicked = false;
			}
			break;
		}
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	}
}