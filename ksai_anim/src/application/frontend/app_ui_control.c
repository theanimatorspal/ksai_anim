#include "app_ui.h"
#include <backend/ui/latex_colors.h>

void handle_file_menu(
	ivec2s file_option,
	float aspect,
	vk_rsrs *rsrs,
	SDL_Event *event,
	kie_Scene *scene,
	renderer_backend *backend,
	int *current_selected,
	vec3 clear_color,
	int viewport_objects_count
)
{
	static vec2 pos = { -0.75, 0.25 };
	static bool move = false;


	static bool add_circle_window = false;
	static bool add_cylinder_window = false;
	static bool world_window = true;
	static bool properties_window = false;


	switch (file_option.x)
	{
	case 0:
		break;
	case 1:
		switch (file_option.y)
		{
		case 0:
			add_circle_window = !add_circle_window;
			break;
		case 1:
			add_cylinder_window = !add_cylinder_window;
			break;
		}
		break;
	case 2:
		switch (file_option.y)
		{
		case 0:
			properties_window = !properties_window;
			break;
		case 1:
			break;
		case 2:
			world_window = !world_window;
			break;
		}
	case 3:
		break;
	case 4:
		break;
	}


	if (world_window)
	{
		static vec2 debug_window_pos = { 0.75, -0.7 }; static bool move_debug_window = false;
		draw_window("World    ", 7, debug_window_pos, aspect, rsrs, event, &move_debug_window);

		char log[KSAI_SMALL_STRING_LENGTH];
		sprintf_s(log, sizeof(char) * 100, "Object Count:%d", scene->objects_count);


		draw_label_window(log, debug_window_pos, rsrs, aspect, 1);
		draw_selector_integer(3, scene->objects_count, aspect, debug_window_pos, rsrs, 2, current_selected);

		draw_label_window("Clear Color", debug_window_pos, rsrs, aspect, 3);
		static vec3 colors[100];
		char select[100][100] = { "brown", "lemon", "grey", "sprbud" };
		glm_vec3_copy(color_SMOKYBLACK, colors[0]);
		glm_vec3_copy(color_LEMONCHIFFON, colors[1]);
		glm_vec3_copy(color_BATTLESHIPGREY, colors[2]);
		glm_vec3_copy(color_MEDIUMSPRINGBUD, colors[3]);

		static int selection = 0;
		draw_selector_window(select, 4, aspect, debug_window_pos, rsrs, 4, &selection);
		glm_vec3_copy(colors[selection], clear_color);

		int Length;
		const Uint8 *KeyboardState = SDL_GetKeyboardState(&Length);
		if (KeyboardState[SDL_SCANCODE_DOWN])
		{
			if (*current_selected > viewport_objects_count)
				*current_selected = *current_selected - 1;
		}

		if (KeyboardState[SDL_SCANCODE_UP])
		{
			if (*current_selected < scene->objects_count - 1)
				*current_selected = *current_selected + 1;
		}

	}


	if (add_circle_window)
	{
		static int selection = 8;
		draw_window("Circle", 6, pos, aspect, rsrs, event, &move);
		draw_label_window("Divisions:", pos, rsrs, aspect, 1);
		draw_selector_integer(4, 64, aspect, pos, rsrs, 2, &selection);
		draw_label_window("Radius:", pos, rsrs, aspect, 3);

		static char x[100] = "0";
		static int place_value = 0;
		static bool should_input = false;
		draw_input_number(aspect, pos, rsrs, 4, x, &should_input, &place_value);
		float s;
		sscanf_s(x, "%f", &s);

		if (draw_button_window("Create", pos, rsrs, aspect, 5))
		{
			kie_Object object;
			kie_Object_init(&object);
			kie_Object_create_circle(
				&object,
				s,
				selection,
				(vec3)
			{
				0, 0, 0
			},
				(vec3)
			{
				0, 0, 0
			},
				true,
				0,
				0
			);
			kie_Scene_add_object(scene, 1, &object);
			copy_scene_to_backend(rsrs, scene, backend);
			add_circle_window = !add_circle_window;
			*current_selected = scene->objects_count - 1;
		}
	}


	if (add_cylinder_window)
	{
		static int divs = 8;
		draw_window("Cylinder", 6, pos, aspect, rsrs, event, &move);
		draw_label_window("Divisions:", pos, rsrs, aspect, 1);
		draw_selector_integer(4, 64, aspect, pos, rsrs, 2, &divs);
		draw_label_window("height", pos, rsrs, aspect, 3);

		static char x[100] = "0";
		static int place_value = 0;
		static bool should_input = false;
		draw_input_number(aspect, pos, rsrs, 4, x, &should_input, &place_value);
		float s;
		sscanf_s(x, "%f", &s);


		if (draw_button_window("Create", pos, rsrs, aspect, 5))
		{
			kie_Object object;
			kie_Object_init(&object);
			kie_Object_create_cylinder(&object, 3, divs, (float) s);
			kie_Scene_add_object(scene, 1, &object);
			copy_scene_to_backend(rsrs, scene, backend);
			add_cylinder_window = !add_cylinder_window;
			*current_selected = scene->objects_count - 1;
		}

		if (draw_button_window("Cancel", pos, rsrs, aspect, 6))
		{
			add_cylinder_window = !add_cylinder_window;
		}
	}

	static vec2 props_pos = { -0.75, -0.75 };
	if (properties_window)
	{
		static bool update = true;
		int i = 1;
		float padd = 0.7;
		draw_window("Props", 20, props_pos, aspect, rsrs, event, &move);

		draw_label_window("PosX", props_pos, rsrs, aspect, i++ * padd);
		static char px[100] = ""; static int place_value_px = 0; static bool should_input_px = false;
		draw_input_number(aspect, props_pos, rsrs, i++ * padd, px, &should_input_px, &place_value_px);

		draw_label_window("PosY", props_pos, rsrs, aspect, i++ * padd);
		static char py[100] = ""; static int place_value_py = 0; static bool should_input_py = false;
		draw_input_number(aspect, props_pos, rsrs, i++ * padd, py, &should_input_py, &place_value_py);

		draw_label_window("PosZ", props_pos, rsrs, aspect, i++ * padd);
		static char pz[100] = ""; static int place_value_pz = 0; static bool should_input_pz = false;
		draw_input_number(aspect, props_pos, rsrs, i++ * padd, pz, &should_input_pz, &place_value_pz);

		draw_label_window("RotX", props_pos, rsrs, aspect, i++ * padd);
		static char rx[100] = ""; static int place_value_rx = 0; static bool should_input_rx = false;
		draw_input_number(aspect, props_pos, rsrs, i++ * padd, rx, &should_input_rx, &place_value_rx);

		draw_label_window("RotY", props_pos, rsrs, aspect, i++ * padd);
		static char ry[100] = ""; static int place_value_ry = 0; static bool should_input_ry = false;
		draw_input_number(aspect, props_pos, rsrs, i++ * padd, ry, &should_input_ry, &place_value_ry);

		draw_label_window("RotZ", props_pos, rsrs, aspect, i++ * padd);
		static char rz[100] = ""; static int place_value_rz = 0; static bool should_input_rz = false;
		draw_input_number(aspect, props_pos, rsrs, i++ * padd, rz, &should_input_rz, &place_value_rz);


		draw_label_window("ScaleX", props_pos, rsrs, aspect, i++ * padd);
		static char sx[100] = ""; static int place_value_sx = 0; static bool should_input_sx = false;
		draw_input_number(aspect, props_pos, rsrs, i++ * padd, sx, &should_input_sx, &place_value_sx);

		draw_label_window("ScaleY", props_pos, rsrs, aspect, i++ * padd);
		static char sy[100] = ""; static int place_value_sy = 0; static bool should_input_sy = false;
		draw_input_number(aspect, props_pos, rsrs, i++ * padd, sy, &should_input_sy, &place_value_sy);

		draw_label_window("ScaleZ", props_pos, rsrs, aspect, i++ * padd);
		static char sz[100] = ""; static int place_value_sz = 0; static bool should_input_sz = false;
		draw_input_number(aspect, props_pos, rsrs, i++ * padd, sz, &should_input_sz, &place_value_sz);

		if (
			should_input_px || should_input_py || should_input_pz ||
			should_input_rx || should_input_ry || should_input_rz ||
			should_input_sx || should_input_sy || should_input_sz
			)
		{
			sscanf_s(px, "%f", &scene->objects[*current_selected].position[0]);
			sscanf_s(py, "%f", &scene->objects[*current_selected].position[1]);
			sscanf_s(pz, "%f", &scene->objects[*current_selected].position[2]);

			sscanf_s(rx, "%f", &scene->objects[*current_selected].rotation[0]);
			sscanf_s(ry, "%f", &scene->objects[*current_selected].rotation[1]);
			sscanf_s(rz, "%f", &scene->objects[*current_selected].rotation[2]);

			sscanf_s(sx, "%f", &scene->objects[*current_selected].scale[0]);
			sscanf_s(sy, "%f", &scene->objects[*current_selected].scale[1]);
			sscanf_s(sz, "%f", &scene->objects[*current_selected].scale[2]);
			update = false;
		}
		else
		{
			sprintf_s(px, sizeof(char) * 100, "%.2f", scene->objects[*current_selected].position[0]);
			sprintf_s(py, sizeof(char) * 100, "%.2f", scene->objects[*current_selected].position[1]);
			sprintf_s(pz, sizeof(char) * 100, "%.2f", scene->objects[*current_selected].position[2]);

			sprintf_s(rx, sizeof(char) * 100, "%.2f", scene->objects[*current_selected].rotation[0]);
			sprintf_s(ry, sizeof(char) * 100, "%.2f", scene->objects[*current_selected].rotation[1]);
			sprintf_s(rz, sizeof(char) * 100, "%.2f", scene->objects[*current_selected].rotation[2]);

			sprintf_s(sx, sizeof(char) * 100, "%.2f", scene->objects[*current_selected].scale[0]);
			sprintf_s(sy, sizeof(char) * 100, "%.2f", scene->objects[*current_selected].scale[1]);
			sprintf_s(sz, sizeof(char) * 100, "%.2f", scene->objects[*current_selected].scale[2]);
			update = true;
		}


	}
}