#include "app_ui.h"
#include <backend/ui/latex_colors.h>
#include <application/loaders/obj_loader.h>
#include "3d_viewport.h"
#include <SDL2/SDL_thread.h>

static void evaluate_Keyframes(kie_Scene *scene, uint32_t frame_time, int layer);
static void evaluate_Keyframes_combined(kie_Scene *scene, uint32_t frame_time);
static void kie_EvalKeyframes(kie_Scene *scene, uint32_t frame_time, int layer, int current_evaluation);

static bool is_thread_about_to_exit = true;
static bool is_thread_running()
{
	return !is_thread_about_to_exit;
}

typedef struct sdl_thread_parameter
{
	kie_Camera *camera;
	kie_Scene *scene;
	renderer_backend *backend;
	SDL_Event *event;
	int *current_selected;
	uint32_t *current_frame_timeline;
	vk_rsrs *rsrs;
	int low_range;
	int range;
	int selected;	 /* camera selection */
	int pipeline_id; /* Pipeline ID */
} sdl_thread_parameter;

static sdl_thread_parameter pars;
static int thread_function(void *data)
{
	sdl_thread_parameter *par = (sdl_thread_parameter *)data;
	for (int i = par->low_range; i < par->range; i++)
	{
		*par->current_frame_timeline = i;
		char file_Name[KSAI_SMALL_STRING_LENGTH] = "";
		sprintf_s(file_Name, sizeof(char) * KSAI_SMALL_STRING_LENGTH, "%s%03d.png", "anim/frame", i);
		evaluate_Keyframes_combined(par->scene, *par->current_frame_timeline);
		threeD_viewport_update(par->camera, par->scene, par->backend, par->rsrs->window, par->event, par->rsrs, *par->current_selected);
		threeD_viewport_render_to_image(
			par->camera,
			par->scene,
			par->backend,
			par->rsrs->window,
			par->event,
			par->rsrs,
			*par->current_selected,
			file_Name,
			par->selected - 1,
			par->pipeline_id
		);
	}
}

bool open_explorer(char *file, char *file_type)
{
	// Initialize the OPENFILENAME structure
	OPENFILENAMEA ofn;
	char szFile[260] = {0};
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
	char szFile[260] = {0};
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
	kie_Camera *camera,
	int *viewport_pipeline)
{
	int Length;
	const Uint8 *KeyboardState = SDL_GetKeyboardState(&Length);
	static bool first_call = true;
	static vec2 pos = {-0.40f, 0.0};
	pos[0] = -0.40f;
	pos[1] = 0.0f;
	static bool should_mode = false;
	static bool move = false;
	static bool add_circle_window = false;
	static bool add_cylinder_window = false;
	static bool add_light_window = false;
	static bool world_window = true;
	static bool properties_window = true;
	static bool timeline_window = true;
	static bool add_obj_file = false;
	static bool render_to_img_window = false;
	static bool add_texture_window = false;
	static bool add_camera_window = false;
	static kie_Object light_object;
	static kie_Object camera_object;

	/* Timeline */
	static int current_frame_timeline = 1;
	static int range_low_timeline = 1;
	static int range_high_timeline = 120;
	static SDL_Thread *rendering_thread;

	if (first_call)
	{
		read_obj_to_kie_Object("res/objs/light.obj", &light_object);

		kie_Object_init(&camera_object);
		glm_vec3_copy((vec3){0.2, 0.2, 0.2}, camera_object.scale);
		glm_vec3_copy((vec3){-2, 2, 2}, camera_object.position);
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
			timeline_window = !timeline_window;
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
		static vec2 debug_window_pos = {0.83, -0.87};
		draw_window("World    ", 20, debug_window_pos, aspect, rsrs, event, &move);

		char log[KSAI_SMALL_STRING_LENGTH];
		sprintf_s(log, sizeof(char) * 100, "Object Count:%d", scene->objects_count);

		draw_label_window(log, debug_window_pos, rsrs, aspect, ii++ * padd);
		draw_selector_integer(3, scene->objects_count, aspect, debug_window_pos, rsrs, ii++ * padd, current_selected);

		sprintf_s(log, sizeof(char) * 100, "Lights Count:%d", scene->lights_count);
		draw_label_window(log, debug_window_pos, rsrs, aspect, ii++ * padd);

		draw_label_window("Clear Color", debug_window_pos, rsrs, aspect, ii++ * padd);
		static vec3 colors[100];
		char select[100][100] = {"brown", "lemon", "grey", "sprbud"};
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

		draw_label_window("Renderer:", debug_window_pos, rsrs, aspect, ii++ * padd);
		static int selection_pipeline = 0;
		draw_selector_var(&selection_pipeline, aspect, debug_window_pos, rsrs, ii++ * padd, 2, "Checker", "KSAI");
		*viewport_pipeline = selection_pipeline;

		draw_label_window("MovableWind", debug_window_pos, rsrs, aspect, ii++ * padd);
		static int should_move = 0;
		draw_selector_var(&should_move, aspect, debug_window_pos, rsrs, ii++ * padd, 2, "NO", "YES");

		if (should_move == 1)
		{
			should_mode = false;
		}
		else if (should_move == 0)
		{
			should_mode = true;
		}

		if (should_mode)
		{
			move = false;
		}

		if(draw_button_window("ZfillScl", debug_window_pos, rsrs, aspect, ii++ * padd))
		{
			glm_vec3_copy((vec3) {0.0f, 0.0f, 0.0f}, scene->objects[*current_selected].scale);
		}
		if(draw_button_window("ZfillPos", debug_window_pos, rsrs, aspect, ii++ * padd))
		{
			glm_vec3_copy((vec3) {0.0f, 0.0f, 0.0f}, scene->objects[*current_selected].position);
		}
		if(draw_button_window("ZfillRot", debug_window_pos, rsrs, aspect, ii++ * padd))
		{
			glm_vec3_copy((vec3) {0.0f, 0.0f, 0.0f}, scene->objects[*current_selected].rotation);
		}
		if(draw_button_window("FrameObject", debug_window_pos, rsrs, aspect, ii++ * padd))
		{
			glm_vec3_copy(scene->objects[*current_selected].position, camera->target);
		}
	}

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
				(vec3){
					0, 0, 0},
				(vec3){
					0, 0, 0},
				true,
				0,
				0);
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
			kie_Object_create_cylinder(&object, 3, divs, (float)s);
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
		glm_vec3_copy((vec3){0.2, 0.2, 0.2}, light_object.scale);
		if (draw_button_window("Create", pos, rsrs, aspect, ii++ * padd))
		{
			kie_Object_add_light_object(&light_object, scene);
			*current_selected = scene->objects_count - 1;
			scene->objects[*current_selected].frames = (kie_Frame *)ksai_Arena_allocate(sizeof(kie_Frame) * KSAI_MAX_NO_OF_KEYFRAMES, &global_object_arena);
			scene->objects[*current_selected].is_light = true;
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
		glm_vec3_copy((vec3){0.2, 0.2, 0.2}, light_object.scale);
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
		draw_window("render", 7, pos, aspect, rsrs, event, &move);

		static int pipeline_id = 0;
		draw_selector_var(&pipeline_id, aspect, pos, rsrs, ii++ * padd, 3, "checker", "ksai", "ray");

		static int selected = 0;
		draw_selector_var(&selected, aspect, pos, rsrs, ii++ * padd, 4, "cam", "cam0", "cam1", "cam2");

		if (draw_button_window("render img", pos, rsrs, aspect, ii++ * padd))
		{
			char fileName[KSAI_SMALL_STRING_LENGTH];
			if (save_explorer(fileName, "Image (*.png)\0*.png\0"))
			{
				strcat(fileName, ".png");
				threeD_viewport_render_to_image(camera, scene, backend, rsrs->window, event, rsrs, *current_selected, fileName, selected - 1, pipeline_id);
				render_to_img_window = !render_to_img_window;
			}
		}

		draw_label_window("Animation", pos, rsrs, aspect, ii++ * padd);
		static int range = 10;
		static int low_range = 0;
		static int high_range = 120;
		low_range = range_low_timeline;
		high_range = range_high_timeline;
		draw_selector_integer(low_range, high_range, aspect, pos, rsrs, ii++ * padd, &range);

		if (draw_button_window("Render Animation", pos, rsrs, aspect, ii++ * padd))
		{
			pars = (sdl_thread_parameter){
				.camera = camera,
				.scene = scene,
				.backend = backend,
				.event = event,
				.current_selected = current_selected,
				.current_frame_timeline = &current_frame_timeline,
				.rsrs = rsrs,
				.low_range = low_range,
				.range = range,
				.selected = selected,
				.pipeline_id = pipeline_id};
			rendering_thread = (SDL_Thread *)SDL_CreateThread(thread_function, "Rendering thread", (void *)&pars);
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

	static vec2 props_pos = {-0.75, -0.85};
	props_pos[0] = -0.75f;
	props_pos[1] = -0.85f;
	if (properties_window)
	{
		static bool update = true;
		int i = 1;
		float padd = 0.7;
		{
			draw_window("Props", 20, props_pos, aspect, rsrs, event, &move);

			draw_label_window("PosX", props_pos, rsrs, aspect, i++ * padd);
			static char px[100] = "";
			static int place_value_px = 0;
			static bool should_input_px = false;
			draw_input_number(aspect, props_pos, rsrs, i++ * padd, px, &should_input_px, &place_value_px);

			draw_label_window("PosY", props_pos, rsrs, aspect, i++ * padd);
			static char py[100] = "";
			static int place_value_py = 0;
			static bool should_input_py = false;
			draw_input_number(aspect, props_pos, rsrs, i++ * padd, py, &should_input_py, &place_value_py);

			draw_label_window("PosZ", props_pos, rsrs, aspect, i++ * padd);
			static char pz[100] = "";
			static int place_value_pz = 0;
			static bool should_input_pz = false;
			draw_input_number(aspect, props_pos, rsrs, i++ * padd, pz, &should_input_pz, &place_value_pz);

			draw_label_window("RotX", props_pos, rsrs, aspect, i++ * padd);
			static char rx[100] = "";
			static int place_value_rx = 0;
			static bool should_input_rx = false;
			draw_input_number(aspect, props_pos, rsrs, i++ * padd, rx, &should_input_rx, &place_value_rx);

			draw_label_window("RotY", props_pos, rsrs, aspect, i++ * padd);
			static char ry[100] = "";
			static int place_value_ry = 0;
			static bool should_input_ry = false;
			draw_input_number(aspect, props_pos, rsrs, i++ * padd, ry, &should_input_ry, &place_value_ry);

			draw_label_window("RotZ", props_pos, rsrs, aspect, i++ * padd);
			static char rz[100] = "";
			static int place_value_rz = 0;
			static bool should_input_rz = false;
			draw_input_number(aspect, props_pos, rsrs, i++ * padd, rz, &should_input_rz, &place_value_rz);

			draw_label_window("ScaleX", props_pos, rsrs, aspect, i++ * padd);
			static char sx[100] = "";
			static int place_value_sx = 0;
			static bool should_input_sx = false;
			draw_input_number(aspect, props_pos, rsrs, i++ * padd, sx, &should_input_sx, &place_value_sx);

			draw_label_window("ScaleY", props_pos, rsrs, aspect, i++ * padd);
			static char sy[100] = "";
			static int place_value_sy = 0;
			static bool should_input_sy = false;
			draw_input_number(aspect, props_pos, rsrs, i++ * padd, sy, &should_input_sy, &place_value_sy);

			draw_label_window("ScaleZ", props_pos, rsrs, aspect, i++ * padd);
			static char sz[100] = "";
			static int place_value_sz = 0;
			static bool should_input_sz = false;
			draw_input_number(aspect, props_pos, rsrs, i++ * padd, sz, &should_input_sz, &place_value_sz);

			draw_label_window("Line", props_pos, rsrs, aspect, i++ * padd);
			static char thickness[100] = "";
			static int place_thickness = 0;
			static bool should_thickness = false;
			draw_input_number(aspect, props_pos, rsrs, i++ * padd, thickness, &should_thickness, &place_thickness);

			if (
				should_input_px || should_input_py || should_input_pz ||
				should_input_rx || should_input_ry || should_input_rz ||
				should_input_sx || should_input_sy || should_input_sz ||
				should_thickness)
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
				sscanf_s(thickness, "%f", &scene->objects[*current_selected].line_thickness);
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
				sprintf_s(thickness, sizeof(char) * 100, "%.2f", scene->objects[*current_selected].line_thickness);
				update = true;
			}
		}

		if (scene->objects[*current_selected].is_light)
		{
			draw_label_window("LIGHT", props_pos, rsrs, aspect, i++ * padd);
			draw_label_window("======", props_pos, rsrs, aspect, i++ * padd);
			draw_label_window("Intensity", props_pos, rsrs, aspect, i++ * padd);
			static char inte[100] = "";
			static int place_value_inte = 0;
			static bool should_input_inte = false;
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
			static char px[100] = "";
			static int place_value_px = 0;
			static bool should_input_px = false;
			draw_input_number(aspect, props_pos, rsrs, i++ * padd, px, &should_input_px, &place_value_px);

			draw_label_window("PosY", props_pos, rsrs, aspect, i++ * padd);
			static char py[100] = "";
			static int place_value_py = 0;
			static bool should_input_py = false;
			draw_input_number(aspect, props_pos, rsrs, i++ * padd, py, &should_input_py, &place_value_py);

			draw_label_window("PosZ", props_pos, rsrs, aspect, i++ * padd);
			static char pz[100] = "";
			static int place_value_pz = 0;
			static bool should_input_pz = false;
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

	if (timeline_window)
	{
		static int CurrentEvaluation = 0;
		static int CurrentAnimationLayer = 1;
		draw_timeline(aspect, rsrs, &current_frame_timeline, &range_low_timeline, &range_high_timeline, scene, *current_selected, &CurrentAnimationLayer, &CurrentEvaluation);

		if (KeyboardState[SDL_SCANCODE_RIGHT])
		{
			if (current_frame_timeline < range_high_timeline)
				current_frame_timeline++;
			else
				current_frame_timeline = range_low_timeline;

			kie_EvalKeyframes(scene, current_frame_timeline, CurrentAnimationLayer, CurrentEvaluation);
		}

		if (KeyboardState[SDL_SCANCODE_LEFT])
		{
			if (current_frame_timeline > range_low_timeline)
				current_frame_timeline--;
			else
				current_frame_timeline = range_high_timeline;
			kie_EvalKeyframes(scene, current_frame_timeline, CurrentAnimationLayer, CurrentEvaluation);
		}

		if (KeyboardState[SDL_SCANCODE_S])
		{
			if (kie_Frame_has(&scene->objects[*current_selected], current_frame_timeline, CurrentAnimationLayer))
			{
				kie_Frame_delete(&scene->objects[*current_selected], current_frame_timeline, CurrentAnimationLayer);
				kie_Frame_set(&scene->objects[*current_selected], current_frame_timeline, CurrentAnimationLayer);
			}
			else
			{
				kie_Frame_set(&scene->objects[*current_selected], current_frame_timeline, CurrentAnimationLayer);
			}
			kie_EvalKeyframes(scene, current_frame_timeline, CurrentAnimationLayer, CurrentEvaluation);
			memset(KeyboardState, 0, sizeof(Uint8) * Length);
		}

		if (KeyboardState[SDL_SCANCODE_X])
		{
			if (kie_Frame_has(&scene->objects[*current_selected], current_frame_timeline, CurrentAnimationLayer))
			{
				kie_Frame_delete(&scene->objects[*current_selected], current_frame_timeline, CurrentAnimationLayer);
			}
			kie_EvalKeyframes(scene, current_frame_timeline, CurrentAnimationLayer, CurrentEvaluation);
			memset(KeyboardState, 0, sizeof(Uint8) * Length);
		}
	}
	move = false;
}

static void kie_EvalKeyframes(kie_Scene *scene, uint32_t frame_time, int layer, int current_evaluation)
{
	if (current_evaluation == 0)
		evaluate_Keyframes(scene, frame_time, layer);
	else
		evaluate_Keyframes_combined(scene, frame_time);
}

static void evaluate_Keyframes(kie_Scene *scene, uint32_t frame_time, int layer)
{
	for (int i = 4; i < scene->objects_count; i++)
	{
		kie_Frame_eval(&scene->objects[i], frame_time, layer);
	}
}

static void evaluate_Keyframes_combined(kie_Scene *scene, uint32_t frame_time)
{
	for (int i = 4; i < scene->objects_count; i++)
	{
		kie_Frame_eval(&scene->objects[i], frame_time, 1);
		kie_Frame_eval_additive(&scene->objects[i], frame_time, 2);
		kie_Frame_eval_additive(&scene->objects[i], frame_time, 3);
	}
}