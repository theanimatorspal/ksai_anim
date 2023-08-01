#include "3d_viewport.h"
#include <stdarg.h>


typedef struct vk_ui
{
	int level;
	/* Arrow Colors */
	bool arrx_s, arry_s, arrz_s;
	ivec3 arrx_nc, arrx_sc; /* Arrow X Normal and Selected Color */
	ivec3 arry_nc, arry_sc;
	ivec3 arrz_nc, arrz_sc;
	ivec3 arrx_c, arry_c, arrz_c;
} vk_ui;

static vk_ui rw_ui;

static void ry_cst_wrld(
	SDL_Window *_wndw,
	float _x,
	float _y,
	mat4 _mvp,
	vec3 ry_dir
)
{
	float x, y, z;
	vec3 ray_nds;
	int width, height;
	SDL_GetWindowSize(_wndw, &width, &height);
	x = (2.0f * _x) / width - 1.0f;
	y = 1 - (2.0f * _y) / height;
	z = 1.0f;
	ray_nds[0] = x;
	ray_nds[1] = y;
	ray_nds[2] = z;
	vec4 screen_pos = { ray_nds[0], ray_nds[1], 1.0f, 1.0f };
	mat4 inv_vp;
	glm_mat4_inv_fast(_mvp, inv_vp);
	glm_mat4_mulv(inv_vp, screen_pos, ry_dir);
	glm_normalize(ry_dir);
}

static void ry_cllsn(
	vec3 _pln_nrml,
	vec3 _ry_dir,
	vec3 _org_pnt,
	vec3 cllsn_pnt
)
{

	if (_pln_nrml[1] == 0)
	{
		float l = _ry_dir[0];
		float m = _ry_dir[1];
		float n = _ry_dir[2];
		float alpha = _org_pnt[0];
		float beta = _org_pnt[1];
		float gamma = _org_pnt[2];
		float t = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		t = -beta / m;
		x = alpha + l * t;
		z = gamma + n * t;
		glm_vec3_copy((vec3) { x, 0, z }, cllsn_pnt);
	}

	if (_pln_nrml[2] == 0)
	{
		float l = _ry_dir[0];
		float m = _ry_dir[1];
		float n = _ry_dir[2];
		float alpha = _org_pnt[0];
		float beta = _org_pnt[1];
		float gamma = _org_pnt[2];
		float t = 1.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		t = -gamma / n;
		x = alpha + l * t;
		y = beta + m * t;
		glm_vec3_copy((vec3) { x, y, 0 }, cllsn_pnt);
	}

	if (_pln_nrml[0] == 0)
	{
		float l = fabs(_ry_dir[0]);
		float m = fabs(_ry_dir[1]);
		float n = fabs(_ry_dir[2]);
		float alpha = fabs(_org_pnt[0]);
		float beta = fabs(_org_pnt[1]);
		float gamma = fabs(_org_pnt[2]);
		float t = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		t = -alpha / l;
		y = beta + m * t;
		z = gamma + n * t;
		glm_vec3_copy((vec3) { 0, y, z }, cllsn_pnt);
	}
}





void threeD_viewport_init(kie_Camera *camera, int args, ...)
{
	kie_Camera_init(camera);
	camera->position[0] = 5.0f;
	camera->position[1] = 5.0f;
	camera->position[2] = 5.0f;
	int objects_count = 2;

	va_list object_list;
	va_start(object_list, args);
	for (int i = 0; i < args; i++)
	{
		kie_Object *obj = va_arg(object_list, kie_Object *);
		float x = va_arg(object_list, double);
		float y = va_arg(object_list, double);
		float z = va_arg(object_list, double);
		glm_vec3_copy((vec3) { x, y, z }, obj->color);
		x = va_arg(object_list, double);
		y = va_arg(object_list, double);
		z = va_arg(object_list, double);
		glm_vec3_copy((vec3) { x, y, z }, obj->position);
		x = va_arg(object_list, double);
		y = va_arg(object_list, double);
		z = va_arg(object_list, double);
		glm_vec3_copy((vec3) { x, y, z }, obj->rotation);
		x = va_arg(object_list, double);
		y = va_arg(object_list, double);
		z = va_arg(object_list, double);
		glm_vec3_copy((vec3) { x, y, z }, obj->scale);

	}
	va_end(object_list);
}

void threeD_viewport_events(
	kie_Camera *camera,
	kie_Scene *scene,
	renderer_backend *backend,
	SDL_Window *window,
	SDL_Event *event,
	vk_rsrs *rsrs,
	int selected_object_index
)
{
	static float old_y = 0;
	static float old_z = 0;
	static float old_x = 0;
	static vec2 prev_mouse_pos = { 0, 0 };
	int xpos, ypos;
	double sensitivity = 0.01;
	uint32_t buttons = SDL_GetMouseState(&xpos, &ypos);

	int Length;
	const Uint8 *KeyboardState = SDL_GetKeyboardState(&Length);


	/* Mouse Pick */
	render_offscreen_begin(rsrs, backend);
	threeD_viewport_draw(camera, scene, backend, rsrs, 3, true);
	render_offscreen_end(rsrs, backend);


	VkBufferImageCopy regions = (VkBufferImageCopy){
	.bufferOffset = 0,
	.bufferRowLength = rsrs->vk_swap_chain_image_extent_2d_.width,
	.bufferImageHeight = 0,
	.imageSubresource = (VkImageSubresourceLayers) {
			.layerCount = 1,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		},
	.imageOffset = (VkOffset3D) {.x = 0, .y = 0, .z = 0},
	.imageExtent = (VkExtent3D){
		rsrs->vk_swap_chain_image_extent_2d_.width,
		rsrs->vk_swap_chain_image_extent_2d_.height,
		1
	},
	};

	//	transition_image_layout_util();
	VkImageMemoryBarrier  barr = (VkImageMemoryBarrier){
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = backend->mspk.img_clr_att_,
		.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT
	};

	vkCmdPipelineBarrier(
		vk_command_buffer_[rsrs->current_frame],
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0,
		NULL,
		0,
		NULL,
		1,
		&barr
	);


	vkCmdCopyImageToBuffer(
		vk_command_buffer_[rsrs->current_frame],
		backend->mspk.img_clr_att_,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		backend->mspk.bfr_,
		1,
		&regions
	);

	VkImageMemoryBarrier barrirer = (VkImageMemoryBarrier){
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = backend->mspk.img_clr_att_,
		.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1,
		.srcAccessMask = 0,
		.dstAccessMask = 0
	};

	VkPipelineStageFlags source_stage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	VkPipelineStageFlags dest_stage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	vkCmdPipelineBarrier(
		vk_command_buffer_[rsrs->current_frame],
		source_stage,
		dest_stage,
		0,
		0,
		NULL,
		0,
		NULL,
		1,
		&barrirer
	);

	uint8_t *arr = (uint8_t *) backend->mspk.data_;
	double mouse_x, mouse_y;
	mouse_x = xpos;
	mouse_y = ypos;
	int mouse_xi = (int) mouse_x;
	int mouse_yi = (int) mouse_y;
	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	int indx = (mouse_yi * width + mouse_xi) * 4;


	if (indx < width * height * 4 && indx >= 0)
	{
		ivec3 clr;
		glm_ivec3_copy((ivec3) { (int) arr[indx], (int) arr[indx + 1], (int) arr[indx + 2] }, clr);

		ivec3 clr_arrx;
		ivec3 clr_arry;
		ivec3 clr_arrz;

		glm_ivec3_copy(
			INT_CLR_CNVRT(scene->objects[0].color[0], scene->objects[0].color[1], scene->objects[0].color[2]),
			clr_arrz
		);
		glm_ivec3_copy(
			INT_CLR_CNVRT(scene->objects[1].color[0], scene->objects[1].color[1], scene->objects[1].color[2]),
			clr_arry
		);
		glm_ivec3_copy(
			INT_CLR_CNVRT(scene->objects[2].color[0], scene->objects[2].color[1], scene->objects[2].color[2]),
			clr_arrx
		);

		if (clr_cmp(clr, clr_arrz))
		{
			rw_ui.arrx_s = true;
		}
		else
		{
			rw_ui.arrx_s = false;
		}

		if (clr_cmp(clr, clr_arry))
		{
			rw_ui.arry_s = true;
		}
		else
		{
			rw_ui.arry_s = false;
		}

		if (clr_cmp(clr, clr_arrx))
		{
			rw_ui.arrz_s = true;
		}
		else
		{
			rw_ui.arrz_s = false;
		}
	}

	/* MOUSE MOVEMENT */
	{
		static vec3 prv_ms_cllsn = { 0 };
		static vec3 prv_ms_cllsn_y = { 0 };
		static mat4 mvp;
		static mat4 projection;
		glm_perspective(camera->fov, camera->w / camera->h, 0.1, 100, projection);
		kie_generate_mvp(projection, camera, GLM_MAT4_IDENTITY, mvp);
		static vec3 ray;
		ry_cst_wrld(rsrs->window, mouse_x, mouse_y, mvp, ray);
		static vec3 cllsn_pnt;
		ry_cllsn((vec3) { 1, 0, 1 }, ray, camera->position, cllsn_pnt);
		static vec3 cllsn_pnt_y;
		ry_cllsn((vec3) { 0, 1, 1 }, ray, camera->position, cllsn_pnt_y);
		cllsn_pnt_y[1] = mouse_y;

		static bool tr_st_x = false;
		static bool tr_st_y = false;
		static bool tr_st_z = false;
		static float delta_x = 0;
		static float delta_y = 0;
		static float delta_z = 0;
		static float org_x = 0.0f;
		static float org_y = 0.0f;
		static float org_z = 0.0f;
		delta_x = cllsn_pnt[0] - prv_ms_cllsn[0];
		delta_z = cllsn_pnt[2] - prv_ms_cllsn[2];
		delta_y = cllsn_pnt_y[1] - prv_ms_cllsn_y[1];

		{
			if ((buttons & SDL_BUTTON_LMASK) && !KeyboardState[SDL_SCANCODE_LALT] && !KeyboardState[SDL_SCANCODE_LSHIFT])
			{
				if ((tr_st_x || rw_ui.arrx_s) && !(tr_st_y || tr_st_z))
				{
					scene->objects[0].position[0] = org_x + delta_x;
					tr_st_x = false;
					tr_st_y = false;
					tr_st_z = false;
					tr_st_x = true;
				}
				if ((tr_st_z || rw_ui.arrz_s) && !(tr_st_x || tr_st_y))
				{
					scene->objects[0].position[2] = org_z + delta_z;
					tr_st_x = false;
					tr_st_y = false;
					tr_st_z = false;
					tr_st_z = true;
				}
				if ((tr_st_y || rw_ui.arry_s) && !(tr_st_x || tr_st_z))
				{
					scene->objects[0].position[1] = org_y - delta_y / 70;
					tr_st_x = false;
					tr_st_y = false;
					tr_st_z = false;
					tr_st_y = true;
				}

				if (tr_st_x || tr_st_y || tr_st_z == true)
				{
					glm_vec3_copy(scene->objects[0].position, scene->objects[selected_object_index].position);
				}
			}
			else
			{
				glm_vec3_copy(cllsn_pnt, prv_ms_cllsn);
				glm_vec3_copy(cllsn_pnt_y, prv_ms_cllsn_y);
				org_x = scene->objects[0].position[0];
				org_y = scene->objects[0].position[1];
				org_z = scene->objects[0].position[2];
				tr_st_x = false;
				tr_st_y = false;
				tr_st_z = false;
				delta_x = 0;
				delta_y = 0;
				delta_z = 0;
				glm_vec3_copy(scene->objects[selected_object_index].position, scene->objects[0].position);
			}
		}

		glm_vec3_copy(scene->objects[0].position, scene->objects[1].position);
		glm_vec3_copy(scene->objects[0].position, scene->objects[2].position);
	}


	/* Camera Controls */


	double deltaX = xpos - prev_mouse_pos[0];
	double deltaY = ypos - prev_mouse_pos[1];

	if (!(xpos == prev_mouse_pos[0] && ypos == prev_mouse_pos[1]))
	{
		bool ShouldZoomCamera = ((buttons) & SDL_BUTTON_LMASK) && (KeyboardState[SDL_SCANCODE_LSHIFT]) && !(KeyboardState[SDL_SCANCODE_LALT]);
		bool ShouldRotateCamera = ((buttons) &SDL_BUTTON_LMASK) && KeyboardState[SDL_SCANCODE_LALT] && (!KeyboardState[SDL_SCANCODE_LSHIFT]);
		if (ShouldRotateCamera)
		{
			mat4 mat;
			vec3 out;
			glm_mat4_identity(mat);
			glm_rotate_at(mat, (vec3) { 0, 0, 0 }, deltaY *sensitivity, camera->right);
			glm_euler_angles(mat, out);

			float yaw = deltaX * sensitivity;
			float pitch = deltaY * 4 * sensitivity;
			camera->position[1] = old_x + pitch;
			camera->rotation[1] = old_y + yaw;

		}
		else if (ShouldZoomCamera)
		{
			glm_vec3_add(
			camera->position,
			(vec3)
			{
				camera->direction[0] * deltaY / 1000,
					camera->direction[1] * deltaY / 1000,
					camera->direction[2] * deltaY / 1000
			},
			camera->position);
		}
		else if (!(buttons & SDL_BUTTON_LMASK))
		{
			prev_mouse_pos[0] = xpos;
			prev_mouse_pos[1] = ypos;
		}
	}
	else
	{
		old_y = camera->rotation[1];
		old_x = camera->position[1];

	}

	vec3 dir;
	glm_vec3_copy((vec3) { 0, 1, 0 }, camera->up);
	glm_vec3_sub(camera->position, camera->target, dir);
	glm_vec3_sub(camera->position, camera->target, camera->direction);
	glm_normalize(camera->direction);
	glm_cross(camera->up, camera->direction, camera->right);
	glm_cross(camera->direction, camera->right, camera->up);
	glm_normalize(camera->right);
	glm_lookat(camera->position, camera->target, camera->up, camera->view);
	glm_rotate_at(camera->view, camera->target, camera->rotation[0], (vec3) { 1, 0, 0 });
	glm_rotate_at(camera->view, camera->target, camera->rotation[1], (vec3) { 0, 1, 0 });
	glm_rotate_at(camera->view, camera->target, camera->rotation[2], (vec3) { 0, 0, 1 });
}

void threeD_viewport_update(
	kie_Camera *camera,
	kie_Scene *scene,
	renderer_backend *backend,
	SDL_Window *window,
	SDL_Event *event,
	vk_rsrs *rsrs,
	int selected_object_index
)
{
	uniforms uni = { 0 };
	glm_vec3_copy((vec3) { 0, 0, 0 }, uni.light1);
	glm_vec3_copy((vec3) { 0, 0, 0 }, uni.light2);
	glm_vec3_copy((vec3) { 0, 0, 0 }, uni.light3);
	glm_vec3_copy((vec3) { 0, 0, 0 }, uni.light4);
	glm_vec3_copy((vec3) { 0, 0, 0 }, uni.light5);
	glm_vec3_copy((vec3) { 0, 0, 0 }, uni.light6);
	glm_vec3_copy((vec3) { 0, 0, 0 }, uni.light7);
	glm_vec3_copy((vec3) { 0, 0, 0 }, uni.light8);

	int x = 0;
	for (int i = 0; i < scene->objects_count; i++)
	{
		if (scene->objects[i].is_light)
		{
			switch (x)
			{
				case 0:
					glm_vec3_copy(scene->objects[i].position, uni.light0);
					uni.lint0 = scene->objects[i].intensity;
					break;
				case 1:
					glm_vec3_copy(scene->objects[i].position, uni.light1);
					uni.lint1 = scene->objects[i].intensity;
					break;
				case 2:
					glm_vec3_copy(scene->objects[i].position, uni.light2);
					uni.lint2 = scene->objects[i].intensity;
					break;
				case 3:
					glm_vec3_copy(scene->objects[i].position, uni.light3);
					uni.lint3 = scene->objects[i].intensity;
					break;
				case 4:
					glm_vec3_copy(scene->objects[i].position, uni.light4);
					uni.lint4 = scene->objects[i].intensity;
					break;
				case 5:
					glm_vec3_copy(scene->objects[i].position, uni.light5);
					uni.lint5 = scene->objects[i].intensity;
					break;
				case 6:
					glm_vec3_copy(scene->objects[i].position, uni.light6);
					uni.lint6 = scene->objects[i].intensity;
					break;
				case 7:
					glm_vec3_copy(scene->objects[i].position, uni.light6);
					uni.lint7 = scene->objects[i].intensity;
					break;
				case 8:
					glm_vec3_copy(scene->objects[i].position, uni.light8);
					uni.lint8 = scene->objects[i].intensity;
					break;
			}
			x++;
		}

	}

	for (int i = 0; i < scene->objects_count; i++)
	{
		mat4 mvp;
		mat4 projection;
		mat4 model;
		glm_mat4_identity(model);
		glm_mat4_identity(projection);
		glm_mat4_identity(mvp);
		glm_perspective(camera->fov, camera->w / camera->h, 0.1, 100, projection);

		kie_Object *cur_object = &scene->objects[i];
		kie_Camera *cur_camera = &scene->objects[i].camera;
		vec3 dir;
		if (scene->objects[i].is_camera)
		{
			glm_vec3_copy(scene->objects[i].position, cur_camera->position);
			glm_vec3_sub(cur_camera->position, cur_camera->target, dir);
			glm_vec3_sub(cur_camera->position, cur_camera->target, cur_camera->direction);
			glm_normalize(cur_camera->direction);
			glm_cross(cur_camera->up, cur_camera->direction, cur_camera->right);
			glm_cross(cur_camera->direction, cur_camera->right, cur_camera->up);
			glm_normalize(dir);
			glm_lookat(cur_camera->position, cur_camera->target, (vec3) { 0, 1, 0 }, cur_camera->view);

			glm_vec3_copy(cur_camera->rotation, scene->objects[i].rotation);
			glm_lookat(cur_camera->position, cur_camera->target, (vec3) { 0, 1, 0 }, model);
			glm_mat4_inv(model, model);

			glm_scale(model, scene->objects[i].scale);
			kie_generate_mvp(projection, camera, model, mvp);
			glm_mat4_copy(mvp, backend->checker_pipeline.pconstant.mvp);
			glm_vec3_copy(scene->objects[i].color, uni.v1);
			glm_mat4_copy(model, uni.model);
			glm_mat4_copy(camera->view, uni.view);
			glm_mat4_copy(projection, uni.proj);
		}
		else
		{
			glm_translate(model, (vec3) { cur_object->position[0], cur_object->position[1], cur_object->position[2] });
			glm_rotate(model, scene->objects[i].rotation[0], (vec3) { 1, 0, 0 });
			glm_rotate(model, scene->objects[i].rotation[1], (vec3) { 0, 1, 0 });
			glm_rotate(model, scene->objects[i].rotation[2], (vec3) { 0, 0, 1 });
			glm_scale(model, scene->objects[i].scale);
			kie_generate_mvp(projection, camera, model, mvp);
			glm_mat4_copy(mvp, backend->checker_pipeline.pconstant.mvp);
			glm_vec3_copy(scene->objects[i].color, uni.v1);
			glm_mat4_copy(model, uni.model);
			glm_mat4_copy(camera->view, uni.view);
			glm_mat4_copy(projection, uni.proj);
		}

		/* This is For Line View */
		mat4 camera_matrix;
		glm_mat4_identity(camera_matrix);
		glm_rotate(camera_matrix, -camera->rotation[1], (vec3) { 0, 1, 0 });
		vec4 view_direction;
		glm_vec3_copy(
			(vec4)
		{
			camera->direction[0],
				camera->direction[1],
				camera->direction[2],
				1.0f
		},
			view_direction
		);
		glm_mat4_mulv(camera_matrix, view_direction, view_direction);
		glm_vec3_copy(
			(vec3)
		{
			view_direction[0],
				view_direction[1],
				view_direction[2]
		},
			uni.view_dir
		);

		uni.line_thickness = cur_object->line_thickness;

		if (i == selected_object_index)
		{
			uni.v2[0] = 1;
		}
		else
		{
			uni.v2[0] = 0.0;
		}

		uni.v2[2] = (float) scene->objects[i].has_texture;

		if (scene->objects[i].is_light)
		{
			uni.v2[1] = 1.0;
		}
		else
		{
			uni.v2[1] = 0.0;
		}

		memcpy(backend->udata[rsrs->current_frame] + i * sizeof(uniforms), &uni, sizeof(uniforms));
	}

}

void threeD_viewport_draw(
	kie_Camera *camera,
	kie_Scene *scene,
	renderer_backend *backend,
	vk_rsrs *rsrs,
	int viewport_obj_count,
	bool only_viewport_objects
)
{
	threeD_viewport_draw_buf(camera, scene, backend, rsrs, viewport_obj_count, only_viewport_objects, vk_command_buffer_[rsrs->current_frame]);
}


void threeD_viewport_render_to_image(
	kie_Camera *camera,
	kie_Scene *scene,
	renderer_backend *backend,
	SDL_Window *window,
	SDL_Event *event,
	vk_rsrs *rsrs,
	int selected_object_index,
	char file[KSAI_SMALL_STRING_LENGTH],
	int camera_id,
	int pipe_id
)
{

	VkCommandBuffer cmd_buffer = begin_single_time_commands_util(rsrs->mRenderCommandPool);

	kie_Camera *cur_camera = NULL;
	int id = -1;
	if (camera_id != -1)
	{
		for (int i = 0; i < scene->objects_count; i++)
		{
			id++;
			if (scene->objects[i].is_camera && id == i)
			{
				cur_camera = &scene->objects[i].camera;
			}
		}
	}
	if (cur_camera == NULL)
	{
		cur_camera = camera;
	}
	threeD_viewport_update(cur_camera, scene, backend, window, event, rsrs, selected_object_index);
	render_offscreen_begin_buf(rsrs, backend, cmd_buffer, (vec3) { 0, 0, 0 });
	draw_skybox_backendbuf(rsrs, backend, scene, 3, cmd_buffer);
	threeD_viewport_draw_buf_without_viewport_and_lights(cur_camera, scene, backend, rsrs, 3, cmd_buffer, pipe_id);
	render_offscreen_end_buf(rsrs, backend, cmd_buffer);


	VkBufferImageCopy regions = (VkBufferImageCopy){
	.bufferOffset = 0,
	.bufferRowLength = rsrs->vk_swap_chain_image_extent_2d_.width,
	.bufferImageHeight = 0,
	.imageSubresource = (VkImageSubresourceLayers) {
			.layerCount = 1,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		},
	.imageOffset = (VkOffset3D) {.x = 0, .y = 0, .z = 0},
	.imageExtent = (VkExtent3D){
		rsrs->vk_swap_chain_image_extent_2d_.width,
		rsrs->vk_swap_chain_image_extent_2d_.height,
		1
	},
	};

	//	transition_image_layout_util();
	VkImageMemoryBarrier  barr = (VkImageMemoryBarrier){
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = backend->mspk.img_clr_att_,
		.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT
	};

	vkCmdPipelineBarrier(
		cmd_buffer,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0,
		NULL,
		0,
		NULL,
		1,
		&barr
	);


	vkCmdCopyImageToBuffer(
		cmd_buffer,
		backend->mspk.img_clr_att_,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		backend->mspk.render_buffer,
		1,
		&regions
	);

	VkImageMemoryBarrier barrirer = (VkImageMemoryBarrier){
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = backend->mspk.img_clr_att_,
		.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1,
		.srcAccessMask = 0,
		.dstAccessMask = 0
	};

	VkPipelineStageFlags source_stage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	VkPipelineStageFlags dest_stage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	vkCmdPipelineBarrier(
		cmd_buffer,
		source_stage,
		dest_stage,
		0,
		0,
		NULL,
		0,
		NULL,
		1,
		&barrirer
	);

	end_single_time_commands_util(&cmd_buffer, rsrs->vk_compute_queue_);
	vkQueueWaitIdle(rsrs->vk_compute_queue_);

	uint8_t *arr = (uint8_t *) backend->mspk.render_buffer_data;
	stbi_write_png(
		file,
		rsrs->vk_swap_chain_image_extent_2d_.width,
		rsrs->vk_swap_chain_image_extent_2d_.height,
		4,
		arr,
		0
	);
}

void threeD_viewport_draw_buf(
	kie_Camera *camera,
	kie_Scene *scene,
	renderer_backend *backend,
	vk_rsrs *rsrs,
	int viewport_obj_count,
	bool only_viewport_objects,
	VkCommandBuffer cmd_buffer
)
{
	for (int i = 0; i < backend->offset_count; i++)
	{
		int x;
		if (!only_viewport_objects)
		{
			x = (i + viewport_obj_count) % (scene->objects_count);
		}
		else
		{
			x = i;
			if (!(i < viewport_obj_count))
				break;
		}

		kie_Object *the_mesh = &scene->objects[x];


		if (x < viewport_obj_count)
		{
			vkCmdSetDepthTestEnable(cmd_buffer, VK_FALSE);
			vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, backend->constant_color.vk_pipeline_);
		}
		else
		{
			vkCmdSetDepthTestEnable(cmd_buffer, VK_TRUE);
			vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, backend->checker_pipeline.vk_pipeline_);
		}

		vkCmdBindDescriptorSets(
			cmd_buffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			backend->checker_pipeline.vk_pipeline_layout_,
			0,
			1,
			&backend->descriptor_sets[x][rsrs->current_frame],
			0,
			NULL
		);
		VkViewport viewport = { 0 };
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = rsrs->vk_swap_chain_image_extent_2d_.width;
		viewport.height = rsrs->vk_swap_chain_image_extent_2d_.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(cmd_buffer, 0, 1, &viewport);

		VkRect2D scissor = { 0 };
		scissor.offset = (VkOffset2D){ 0, 0 };
		scissor.extent = rsrs->vk_swap_chain_image_extent_2d_;
		vkCmdSetScissor(cmd_buffer, 0, 1, &scissor);

		VkBuffer vertex_buffers[] = { backend->vbuffer };
		VkDeviceSize offsets[] = { backend->voffsets[x] };
		vkCmdBindVertexBuffers(cmd_buffer, 0, 1, vertex_buffers, offsets);

		vkCmdBindIndexBuffer(cmd_buffer, backend->ibuffer, backend->ioffsets[x], VK_INDEX_TYPE_UINT32);

		mat4 mvp;
		mat4 projection;
		mat4 model;
		glm_mat4_identity(model);
		glm_mat4_identity(projection);
		glm_mat4_identity(mvp);
		glm_perspective(camera->fov, camera->w / camera->h, 0.1, 100, projection);
		glm_translate(model, the_mesh->position);
		glm_rotate(model, the_mesh->rotation[0], (vec3) { 1, 0, 0 });
		glm_rotate(model, the_mesh->rotation[1], (vec3) { 0, 1, 0 });
		glm_rotate(model, the_mesh->rotation[2], (vec3) { 0, 0, 1 });
		glm_scale(model, the_mesh->scale);
		kie_generate_mvp(projection, camera, model, mvp);
		glm_mat4_copy(mvp, backend->checker_pipeline.pconstant.mvp);
		vkCmdPushConstants(cmd_buffer, backend->checker_pipeline.vk_pipeline_layout_, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(backend->checker_pipeline.pconstant), &backend->checker_pipeline.pconstant);
		if (x != 3) vkCmdDrawIndexed(cmd_buffer, the_mesh->indices_count, 1, 0, 0, 0);
		vkCmdSetDepthTestEnable(cmd_buffer, VK_FALSE);
	}
}


/* OFFSCREEN Rendering to Image */
void threeD_viewport_draw_buf_without_viewport_and_lights(
	kie_Camera *camera,
	kie_Scene *scene,
	renderer_backend *backend,
	vk_rsrs *rsrs,
	int viewport_obj_count,
	VkCommandBuffer cmd_buffer,
	int pipeline_id
)
{
	pipeline_vk *pipe;
	switch (pipeline_id)
	{
		case 0:
			pipe = &backend->checker_pipeline;
			break;
		case 1:
			pipe = &backend->ksai_render_pipeline;
			break;
	}

	for (int i = viewport_obj_count; i < backend->offset_count; i++)
	{
		int x = i;

		kie_Object *the_mesh = &scene->objects[x];
		if (x == 3) // for skybox
			continue;
		if (the_mesh->is_light)
			continue;
		if (the_mesh->is_camera)
			continue;


		vkCmdSetDepthTestEnable(cmd_buffer, VK_TRUE);
		vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipe->vk_pipeline_);

		vkCmdBindDescriptorSets(
			cmd_buffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipe->vk_pipeline_layout_,
			0,
			1,
			&backend->descriptor_sets[x][rsrs->current_frame],
			0,
			NULL
		);
		VkViewport viewport = { 0 };
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = rsrs->vk_swap_chain_image_extent_2d_.width;
		viewport.height = rsrs->vk_swap_chain_image_extent_2d_.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(cmd_buffer, 0, 1, &viewport);

		VkRect2D scissor = { 0 };
		scissor.offset = (VkOffset2D){ 0, 0 };
		scissor.extent = rsrs->vk_swap_chain_image_extent_2d_;
		vkCmdSetScissor(cmd_buffer, 0, 1, &scissor);

		VkBuffer vertex_buffers[] = { backend->vbuffer };
		VkDeviceSize offsets[] = { backend->voffsets[x] };
		vkCmdBindVertexBuffers(cmd_buffer, 0, 1, vertex_buffers, offsets);

		vkCmdBindIndexBuffer(cmd_buffer, backend->ibuffer, backend->ioffsets[x], VK_INDEX_TYPE_UINT32);


		mat4 mvp;
		mat4 projection;
		mat4 model;
		glm_mat4_identity(model);
		glm_mat4_identity(projection);
		glm_mat4_identity(mvp);
		glm_perspective(camera->fov, camera->w / camera->h, 0.1, 100, projection);
		glm_translate(model, the_mesh->position);
		glm_rotate(model, the_mesh->rotation[0], (vec3) { 1, 0, 0 });
		glm_rotate(model, the_mesh->rotation[1], (vec3) { 0, 1, 0 });
		glm_rotate(model, the_mesh->rotation[2], (vec3) { 0, 0, 1 });
		glm_scale(model, the_mesh->scale);
		kie_generate_mvp(projection, camera, model, mvp);
		glm_mat4_copy(mvp, pipe->pconstant.mvp);
		vkCmdPushConstants(cmd_buffer, pipe->vk_pipeline_layout_, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(pipe->pconstant), &pipe->pconstant);
		vkCmdDrawIndexed(cmd_buffer, the_mesh->indices_count, 1, 0, 0, 0);
		vkCmdSetDepthTestEnable(cmd_buffer, VK_FALSE);
	}
}

