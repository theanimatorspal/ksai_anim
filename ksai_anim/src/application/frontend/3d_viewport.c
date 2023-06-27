#include <engine/renderer/scene.h>
#include <backend/vulkan/backend.h>
#include <vendor/cglm/cglm.h>
#include "3d_viewport.h"

void threeD_viewport_init(kie_Camera *camera)
{
	camera->direction[0] = 0.0f;
	camera->direction[1] = 0.0f;
	camera->direction[2] = 0.0f;
	camera->target[0] = 0.0f;
	camera->target[1] = 0.0f;
	camera->target[2] = 0.0f;
	camera->up[0] = 0.0f;
	camera->up[1] = 1.0f;
	camera->up[2] = 0.0f;
	camera->position[0] = 5.0f;
	camera->position[1] = 5.0f;
	camera->position[2] = 5.0f;
	camera->rotation[0] = 0.0f;
	camera->rotation[1] = 0.0f;
	camera->rotation[2] = 0.0f;
	glm_vec3_copy((vec3) { 0, 0, 0 }, camera->pivot);
	camera->fov = 0.7863;
	camera->w = 1920;
	camera->h = 1080;
}

void threeD_viewport_events(kie_Camera *camera, SDL_Window *window, SDL_Event *event)
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


	double deltaX = xpos - prev_mouse_pos[0];
	double deltaY = ypos - prev_mouse_pos[1];

	if (!(xpos == prev_mouse_pos[0] && ypos == prev_mouse_pos[1]))
	{
		if ((buttons & SDL_BUTTON_LMASK) && KeyboardState[SDL_SCANCODE_LALT])
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
	glm_vec3_sub(camera->position, camera->target, dir);
	glm_vec3_sub(camera->position, camera->target, camera->direction);
	glm_normalize(camera->direction);
	glm_cross(camera->up, camera->direction, camera->right);
	glm_cross(camera->direction, camera->right, camera->up);
	glm_normalize(camera->right);
	glm_lookat(camera->position, camera->target, camera->up, camera->view);
	glm_rotate_at(camera->view, camera->pivot, camera->rotation[0], (vec3) { 1, 0, 0 });
	glm_rotate_at(camera->view, camera->pivot, camera->rotation[1], (vec3) { 0, 1, 0 });
	glm_rotate_at(camera->view, camera->pivot, camera->rotation[2], (vec3) { 0, 0, 1 });
}

void threeD_viewport_draw(kie_Camera *camera, kie_Scene *scene, renderer_backend *backend, vk_rsrs *rsrs)
{
	for (int i = 0; i < scene->objects_count; i++)
	{
		vkCmdSetDepthTestEnable(vk_command_buffer_[rsrs->current_frame], VK_TRUE);
		kie_Object *the_mesh = &scene->objects[i];
		vkCmdBindPipeline(vk_command_buffer_[rsrs->current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, backend->checker_pipeline.vk_pipeline_);

		vkCmdBindDescriptorSets(vk_command_buffer_[rsrs->current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, backend->checker_pipeline.vk_pipeline_layout_, 0, 1, &backend->checker_pipeline.vk_descriptor_sets_[rsrs->current_frame], 0, NULL);
		VkViewport viewport = { 0 };
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = rsrs->vk_swap_chain_image_extent_2d_.width;
		viewport.height = rsrs->vk_swap_chain_image_extent_2d_.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(vk_command_buffer_[rsrs->current_frame], 0, 1, &viewport);

		VkRect2D scissor = { 0 };
		scissor.offset = (VkOffset2D){ 0, 0 };
		scissor.extent = rsrs->vk_swap_chain_image_extent_2d_;
		vkCmdSetScissor(vk_command_buffer_[rsrs->current_frame], 0, 1, &scissor);

		VkBuffer vertex_buffers[] = { backend->vbuffer };
		VkDeviceSize offsets[] = { backend->voffsets[i]};
		vkCmdBindVertexBuffers(vk_command_buffer_[rsrs->current_frame], 0, 1, vertex_buffers, offsets);

		vkCmdBindIndexBuffer(vk_command_buffer_[rsrs->current_frame], backend->ibuffer, backend->ioffsets[i], VK_INDEX_TYPE_UINT32);


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
		vkCmdPushConstants(vk_command_buffer_[rsrs->current_frame], backend->checker_pipeline.vk_pipeline_layout_, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(backend->checker_pipeline.pconstant), &backend->checker_pipeline.pconstant);
		vkCmdDrawIndexed(vk_command_buffer_[rsrs->current_frame], the_mesh->indices_count, 1, 0, 0, 0);
		vkCmdSetDepthTestEnable(vk_command_buffer_[rsrs->current_frame], VK_FALSE);

	}
}