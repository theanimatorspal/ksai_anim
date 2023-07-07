#include "app_ui.h"
#include <backend/ui/latex_colors.h>
#include <application/loaders/obj_loader.h>
#include "3d_viewport.h"

bool open_explorer(char *file, char *file_type)
{
	// Initialize the OPENFILENAME structure
	OPENFILENAMEA ofn;
	char szFile[260] = { 0 };
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = file_type;
	ofn.lpstrTitle = "Select a file";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	// Display the common file dialog to get the file path
	if (GetOpenFileNameA(&ofn) == TRUE)
	{
		// The user selected a file, so print its path
		printf("Selected file: %s\n", szFile);
		sprintf(file, "%s", szFile);
		return true;
	}
	else
	{
		// The user cancelled the dialog, so handle the error
		printf("Error: %d\n", CommDlgExtendedError());
		return false;
	}
}

bool save_explorer(char *file, char *file_type)
{
	// Initialize the OPENFILENAME structure
	OPENFILENAMEA ofn;
	char szFile[260] = { 0 };
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = file_type;
	ofn.lpstrTitle = "Select a file";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	// Display the common file dialog to get the file path
	if (GetSaveFileNameA(&ofn) == TRUE)
	{
		// The user selected a file, so print its path
		printf("Saved file: %s\n", szFile);
		sprintf(file, "%s", szFile);
		return true;
	}
	else
	{
		// The user cancelled the dialog, so handle the error
		printf("Error: %d\n", CommDlgExtendedError());
		return false;
	}
}


void handle_file_menu(
	ivec2s file_option,
	float aspect,
	vk_rsrs *rsrs,
	SDL_Event *event,
	kie_Scene *scene,
	renderer_backend *backend,
	int *current_selected,
	vec3 clear_color,
	int viewport_objects_count,
	bool *should_show_viewport_objects,
	kie_Camera *camera
)
{
	static bool first_call = true;
	static vec2 pos = { -0.40, 0.35 };
	static bool move = false;
	static bool add_circle_window = false;
	static bool add_cylinder_window = false;
	static bool add_light_window = false;
	static bool world_window = true;
	static bool properties_window = false;
	static bool add_obj_file = false;
	static bool render_to_img_window = false;
	static bool add_texture_window = false;
	static bool add_camera_window = false;
	static kie_Object light_object;
	static kie_Object camera_object;

	if (first_call)
	{
		read_obj_to_kie_Object("res/objs/light.obj", &light_object);

		kie_Object_init(&camera_object);
		glm_vec3_copy((vec3) { 0.2, 0.2, 0.2 }, camera_object.scale);
		glm_vec3_copy((vec3) { -2, 2, 2 }, camera_object.position);
		kie_Camera_init(&camera_object.camera);
		read_obj_to_kie_Object("res/objs/camera.obj", &camera_object);
		camera_object.is_camera = true;


		light_object.intensity = 1.0f;

		first_call = false;
	}

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
				case 3:
					add_obj_file = !add_obj_file;
					break;
				case 4:
					add_light_window = !add_light_window;
					break;
				case 5:
					add_camera_window = !add_camera_window;
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
			break;
		case 3:
			switch (file_option.y)
			{
				case 0:
					add_texture_window = !add_texture_window;
					break;
			}
			break;
		case 4:
			switch (file_option.y)
			{
				case 0:
					render_to_img_window = !render_to_img_window;
					break;
			}
			break;
	}

	if (world_window)
	{
		int ii = 2;
		float padd = 0.7;
		static vec2 debug_window_pos = { 0.75, -0.7 }; static bool move_debug_window = false;
		draw_window("World    ", 7, debug_window_pos, aspect, rsrs, event, &move_debug_window);

		char log[KSAI_SMALL_STRING_LENGTH];
		sprintf_s(log, sizeof(char) * 100, "Object Count:%d", scene->objects_count);


		draw_label_window(log, debug_window_pos, rsrs, aspect, ii++ * padd);
		draw_selector_integer(3, scene->objects_count, aspect, debug_window_pos, rsrs, ii++ * padd, current_selected);

		sprintf_s(log, sizeof(char) * 100, "Lights Count:%d", scene->lights_count);
		draw_label_window(log, debug_window_pos, rsrs, aspect, ii++ * padd);

		draw_label_window("Clear Color", debug_window_pos, rsrs, aspect, ii++ * padd);
		static vec3 colors[100];
		char select[100][100] = { "brown", "lemon", "grey", "sprbud" };
		glm_vec3_copy(color_SMOKYBLACK, colors[0]);
		glm_vec3_copy(color_LEMONCHIFFON, colors[1]);
		glm_vec3_copy(color_BATTLESHIPGREY, colors[2]);
		glm_vec3_copy(color_MEDIUMSPRINGBUD, colors[3]);

		static int selection = 0;
		draw_selector_window(select, 4, aspect, debug_window_pos, rsrs, ii++ * padd, &selection);
		glm_vec3_copy(colors[selection], clear_color);

		draw_label_window("Show VOs:", debug_window_pos, rsrs, aspect, ii++ * padd);
		static int selection2 = 0;
		draw_selector_var(&selection2, aspect, debug_window_pos, rsrs, ii++ * padd, 2, "__TRUE__", "__FALSE__");

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

	if (add_light_window)
	{
		int ii = 2;
		float padd = 0.7;

		draw_window("Light", 4, pos, aspect, rsrs, event, &move);
		glm_vec3_copy((vec3) { 0.2, 0.2, 0.2 }, light_object.scale);
		if (draw_button_window("Create", pos, rsrs, aspect, ii++ * padd))
		{
			kie_Object_add_light_object(&light_object, scene);
			*current_selected = scene->objects_count - 1;
			copy_scene_to_backend(rsrs, scene, backend);
			add_light_window = !add_light_window;
		}

		if (draw_button_window("Cancel", pos, rsrs, aspect, ii++ * padd))
		{
			add_light_window = !add_light_window;
		}
	}

	if (add_camera_window)
	{
		int ii = 2;
		float padd = 0.7;
		draw_window("Camera", 4, pos, aspect, rsrs, event, &move);
		glm_vec3_copy((vec3) { 0.2, 0.2, 0.2 }, light_object.scale);
		if (draw_button_window("Create", pos, rsrs, aspect, ii++ * padd))
		{
			kie_Scene_add_object(scene, 1, &camera_object);
			copy_scene_to_backend(rsrs, scene, backend);
			*current_selected = scene->objects_count - 1;
			add_camera_window = !add_camera_window;
		}
		if (draw_button_window("Cancel", pos, rsrs, aspect, ii++ * padd))
		{
			add_camera_window = !add_camera_window;
		}

	}

	if (add_obj_file)
	{
		int ii = 2;
		float padd = 0.7;
		draw_window("add", 4, pos, aspect, rsrs, event, &move);
		if (draw_button_window("Add", pos, rsrs, aspect, ii++ * padd))
		{
			char filepath[KSAI_SMALL_STRING_LENGTH];
			if (open_explorer(filepath, "WaveFront OBJ (*.obj)\0*.obj\0"))
			{
				kie_Object object;
				kie_Object_init(&object);
				read_obj_to_kie_Object(filepath, &object);
				kie_Scene_add_object(scene, 1, &object);
				copy_scene_to_backend(rsrs, scene, backend);
			}
			add_obj_file = !add_obj_file;
			*current_selected = scene->objects_count - 1;

		}
		if (draw_button_window("Add_rec", pos, rsrs, aspect, ii++ * padd))
		{
			char filepath[KSAI_SMALL_STRING_LENGTH];
			if (open_explorer(filepath, "WaveFront OBJ (*.obj)\0*.obj\0"))
			{
				read_add_auto_objs(scene, filepath);
				copy_scene_to_backend(rsrs, scene, backend);
			}
			add_obj_file = !add_obj_file;
			*current_selected = scene->objects_count - 1;

		}
		if (draw_button_window("Cancel", pos, rsrs, aspect, ii++ * padd))
		{
			add_obj_file = !add_obj_file;
		}
	}

	if (render_to_img_window)
	{
		int ii = 2;
		float padd = 0.7;
		draw_window("render", 6, pos, aspect, rsrs, event, &move);

		static int stuff = 0;
		draw_selector_var(&stuff, aspect, pos, rsrs, ii++ * padd, 3, "checker", "ksai", "ray");

		static int selected = 0;
		draw_selector_var(&selected, aspect, pos, rsrs, ii++ * padd, 4, "cam", "cam0", "cam1", "cam2");

		if (draw_button_window("render img", pos, rsrs, aspect, ii++ * padd))
		{
			char fileName[KSAI_SMALL_STRING_LENGTH];
			if (save_explorer(fileName, "Image (*.png)\0*.png\0"))
			{
				strcat(fileName, ".png");
				threeD_viewport_render_to_image(camera, scene, backend, rsrs->window, event, rsrs, *current_selected, fileName, selected - 1);
				render_to_img_window = !render_to_img_window;
			}

		}
		if (draw_button_window("Cancel", pos, rsrs, aspect, ii++ * padd))
		{
			render_to_img_window = !render_to_img_window;
		}

	}

	if (add_texture_window)
	{
		int ii = 2;
		float padd = 0.7;
		draw_window("render", 4, pos, aspect, rsrs, event, &move);
		if (draw_button_window("Choose", pos, rsrs, aspect, ii++ * padd))
		{
			char filepath[KSAI_SMALL_STRING_LENGTH];
			if (open_explorer(filepath, "Image (*.png)\0*.png\0"))
			{
				backend_add_texture_to_scene_object(rsrs, backend, scene, *current_selected, filepath);
				copy_scene_to_backend_reload(rsrs, scene, backend);
			}
			add_texture_window = !add_texture_window;

		}


	}

	static vec2 props_pos = { -0.75, -0.75 };
	if (properties_window)
	{
		static bool update = true;
		int i = 1;
		float padd = 0.7;
		{
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

		if (scene->objects[*current_selected].is_light)
		{
			draw_label_window("LIGHT", props_pos, rsrs, aspect, i++ * padd);
			draw_label_window("======", props_pos, rsrs, aspect, i++ * padd);
			draw_label_window("Intensity", props_pos, rsrs, aspect, i++ * padd);
			static char inte[100] = ""; static int place_value_inte = 0; static bool should_input_inte = false;
			draw_input_number(aspect, props_pos, rsrs, i++ * padd, inte, &should_input_inte, &place_value_inte);

			if (should_input_inte)
			{
				sscanf_s(inte, "%f", &scene->objects[*current_selected].intensity);
			}
			else
			{
				sprintf_s(inte, sizeof(char) * 100, "%.2f", scene->objects[*current_selected].intensity);
			}
		}


		if (scene->objects[*current_selected].is_camera)
		{
			draw_label_window("Camera", props_pos, rsrs, aspect, i++ * padd);
			draw_label_window("======", props_pos, rsrs, aspect, i++ * padd);
			draw_label_window("Target", props_pos, rsrs, aspect, i++ * padd);

			draw_label_window("PosX", props_pos, rsrs, aspect, i++ * padd);
			static char px[100] = ""; static int place_value_px = 0; static bool should_input_px = false;
			draw_input_number(aspect, props_pos, rsrs, i++ * padd, px, &should_input_px, &place_value_px);

			draw_label_window("PosY", props_pos, rsrs, aspect, i++ * padd);
			static char py[100] = ""; static int place_value_py = 0; static bool should_input_py = false;
			draw_input_number(aspect, props_pos, rsrs, i++ * padd, py, &should_input_py, &place_value_py);

			draw_label_window("PosZ", props_pos, rsrs, aspect, i++ * padd);
			static char pz[100] = ""; static int place_value_pz = 0; static bool should_input_pz = false;
			draw_input_number(aspect, props_pos, rsrs, i++ * padd, pz, &should_input_pz, &place_value_pz);

			kie_Camera *current_cam = &scene->objects[*current_selected].camera;
			if (should_input_px || should_input_py || should_input_pz)
			{
				sscanf_s(px, "%f", &current_cam->target[0]);
				sscanf_s(py, "%f", &current_cam->target[1]);
				sscanf_s(pz, "%f", &current_cam->target[2]);
			}
			else
			{
				sprintf_s(px, sizeof(char) * 100, "%.2f", current_cam->target[0]);
				sprintf_s(py, sizeof(char) * 100, "%.2f", current_cam->target[1]);
				sprintf_s(pz, sizeof(char) * 100, "%.2f", current_cam->target[1]);
			}

		}



	}
}