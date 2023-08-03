#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include <stdio.h>
#include <stdbool.h>
#include "ui.h"
#include "font.h"


static global_ui_base_pipeline base_;
static ui_label global_all_labels[MAX_BUFFER_SIZE];
static uint32_t global_all_labels_count = 0;
static fnt_d global_base_font;


void ui_init(int sz_fctr, vk_rsrs *_rsrs)
{

	static bool first_time = true;
	if (first_time == true)
	{

		ui_label label;
		strcpy(label.text, "Everything in this world is fucking different");
		base_.ppln.vertices = label.vrtcs;
		base_.ppln.vertices_count = 4;
		base_.ppln.indcs = label.indcs;
		base_.ppln.indices_count = 6;
		float size = 0.5;
		glm_vec3_copy((vec3) { -1 * size, -1 * size, 0 }, label.vrtcs[0].pos);
		glm_vec3_copy((vec3) { -1 * size, 1 * size, 0 }, label.vrtcs[1].pos);
		glm_vec3_copy((vec3) { 1 * size, 1 * size, 0 }, label.vrtcs[2].pos);
		glm_vec3_copy((vec3) { 1 * size, -1 * size, 0 }, label.vrtcs[3].pos);
		label.indcs[0] = 0;
		label.indcs[1] = 1;
		label.indcs[2] = 2;
		label.indcs[3] = 0;
		label.indcs[4] = 2;
		label.indcs[5] = 3;
		VkDescriptorSetLayoutBinding ubo_layout_binding = { 0 };
		ubo_layout_binding = (VkDescriptorSetLayoutBinding){
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			.pImmutableSamplers = NULL
		};

		VkDescriptorSetLayoutBinding sampler_layout_binding = { 0 };
		sampler_layout_binding = (VkDescriptorSetLayoutBinding){
			.binding = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
			.pImmutableSamplers = NULL
		};

		VkDescriptorSetLayoutBinding bindings[2] = { ubo_layout_binding, sampler_layout_binding };


		VkVertexInputBindingDescription binding_desp = (VkVertexInputBindingDescription){
			.binding = 0,
			.stride = sizeof(struct l_vbs_vrtx),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		};
		VkVertexInputAttributeDescription attr_desp[3];

		attr_desp[0] = (VkVertexInputAttributeDescription){
			.binding = 0,
			.location = 0,
			.format = VK_FORMAT_R32G32B32_SFLOAT,
			.offset = offsetof(struct l_vbs_vrtx, pos)
		};

		attr_desp[1] = (VkVertexInputAttributeDescription){
			.binding = 0,
			.location = 1,
			.format = VK_FORMAT_R32G32B32_SFLOAT,
			.offset = offsetof(struct l_vbs_vrtx, normal)
		};

		attr_desp[2] = (VkVertexInputAttributeDescription){
			.binding = 0,
			.location = 2,
			.format = VK_FORMAT_R32G32_SFLOAT,
			.offset = offsetof(struct l_vbs_vrtx, tex_coords)
		};


		VkDescriptorPoolSize pool_sizes[2] = { 0 };
		pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		pool_sizes[0].descriptorCount = MAX_FRAMES_IN_FLIGHT;
		pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_sizes[1].descriptorCount = MAX_FRAMES_IN_FLIGHT;
		prs_bm_fnt("res/font/font.fnt", &base_.fnt);
		prs_bm_fnt("res/font/font.fnt", &global_base_font);

		create_vulkan_pipeline(
			_rsrs,
			&base_.ppln,
			1,
			bindings,
			"res/shaders/ui/button/vshader.spv",
			"res/shaders/ui/button/fshader.spv",
			&binding_desp,
			1,
			attr_desp,
			3,
			0,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			1,
			pool_sizes,
			0,
			0
		);


		strcpy(base_.fnt.fnt_img_pth, "res/font/font.png");
		char *d[1 << 8] = { base_.fnt.fnt_img_pth };
		gn_txt(label.text, strlen(label.text), &base_.fnt);
		gn_txt(label.text, strlen(label.text), &global_base_font);
		create_vulkan_pipeline(
			_rsrs,
			&base_.fnt.ppln,
			2,
			bindings,
			"res/shaders/ui/text/vshader.spv",
			"res/shaders/ui/text/fshader.spv",
			&binding_desp,
			1,
			attr_desp,
			3,
			1,
			&base_.fnt.fnt_img,
			&base_.fnt.fnt_img_mmry,
			&base_.fnt.fnt_img_smplr,
			&base_.fnt.fnt_img_vw,
			d,
			2,
			pool_sizes,
			STAGING_BUFFER_VERTEX_ON | STAGING_BUFFER_INDEX_ON,
			sz_fctr
		);
		first_time = false;
	}
}

void ui_get(ui_label lbl, global_ui_base_pipeline *_bttn, VkDeviceSize *_sz, VkDeviceSize *_nsz, int i)
{
	gn_txt(lbl.text, strlen(lbl.text), &_bttn->fnt);
	VkDeviceSize size = _bttn->fnt.ppln.vertices_count * sizeof(l_vbs_vrtx);
	VkDeviceSize nsize = _bttn->fnt.ppln.indices_count * sizeof(_bttn->fnt.ppln.indcs[0]);
	static VkDeviceSize vsize_xt = 0;
	static VkDeviceSize nsize_xt = 0;
	if (i == 0)
	{
		*_sz = 0;
		vsize_xt = size;
	}
	else
	{
		*_sz = vsize_xt;
		vsize_xt += size;
	}

	if (i == 0)
	{
		*_nsz = 0;
		nsize_xt = nsize;
	}
	else
	{
		*_nsz = nsize_xt;
		nsize_xt += nsize;
	}


	mat4 model;
	{
		glm_mat4_identity(_bttn->fnt.ppln.pconstant.mvp);
		glm_mat4_identity(model);
		glm_translate(model, (vec3) { lbl.ps[0], lbl.ps[1], 0 });
		glm_scale(model, lbl.scale);
		glm_mat4_copy(model, _bttn->fnt.ppln.pconstant.mvp);
	}

	/* For Outer Rectangle */
	{
		/* Get Font Features */
		glm_mat4_identity(_bttn->ppln.pconstant.mvp);
		glm_mat4_identity(model);
		glm_translate(model, (vec3) { lbl.ps[0], lbl.ps[1], 0 });
		glm_scale(model, lbl.scale);
		glm_scale(model, (vec3) { _bttn->fnt.max_w * 1.4, _bttn->fnt.max_h * 1.7, 0 });
		glm_mat4_copy(model, _bttn->ppln.pconstant.mvp);
	}
}

void ui_update_vbuffers(ui_label lbl, global_ui_base_pipeline *_bttn, int i, int _crrnt_frm, bool map_it, VkDeviceSize *_sz, VkDeviceSize *_nsz
)
{
	void *data;
	void *ndata;


	/* Update Uniform Buffer */
	{
		memcpy(_bttn->ppln.vk_uniform_buffer_mapped_region_data_[_crrnt_frm], &_bttn->ppln.ubo, sizeof(_bttn->ppln.ubo));
	}
	static VkDeviceSize vsize_xt = 0;
	static VkDeviceSize nsize_xt = 0;
	static bool first_time = true;
	/* For Inner */
	switch (lbl.st_typ)
	{
	case lbl_st_HOVERING:
		glm_vec3_copy(lbl.hvrd_clr, _bttn->ppln.ubo.v1);
		break;
	case lbl_st_UNSELECTED:
		glm_vec3_copy(lbl.nrml_clr, _bttn->ppln.ubo.v1);
		break;
	case lbl_st_SELECTED:
		glm_vec3_copy(lbl.slctd_clr, _bttn->ppln.ubo.v1);
		break;
	}

	glm_vec3_copy(lbl.hvrd_clr, _bttn->ppln.ubo.v2);
	glm_vec3_copy(lbl.nrml_clr, _bttn->ppln.ubo.v3);
	gn_txt(lbl.text, strlen(lbl.text), &_bttn->fnt);

	if (first_time == false && i == 0)
	{
		vsize_xt = 0;
		nsize_xt = 0;
	}

	VkDeviceSize size = _bttn->fnt.ppln.vertices_count * sizeof(l_vbs_vrtx);

	vkMapMemory(vk_logical_device_, _bttn->fnt.ppln.vk_vertex_buffer_memory_, vsize_xt, size, 0, &data);
	if (i == 0)
	{
		*_sz = 0;
		vsize_xt = size;
	}
	else
	{
		*_sz = vsize_xt;
		vsize_xt += size;
	}
	//memset(data, 0, size);
	memcpy(data, _bttn->fnt.ppln.vertices, (size_t) size);
	vkUnmapMemory(vk_logical_device_, _bttn->fnt.ppln.vk_vertex_buffer_memory_);

	VkDeviceSize nsize = _bttn->fnt.ppln.indices_count * sizeof(_bttn->fnt.ppln.indcs[0]);

	vkMapMemory(vk_logical_device_, _bttn->fnt.ppln.vk_index_buffer_memory_, nsize_xt, nsize, 0, &ndata);
	memcpy(ndata, _bttn->fnt.ppln.indcs, (size_t) nsize);

	if (i == 0)
	{
		*_nsz = 0;
		nsize_xt = nsize;
	}
	else
	{
		*_nsz = nsize_xt;
		nsize_xt += nsize;
	}
	vkUnmapMemory(vk_logical_device_, _bttn->fnt.ppln.vk_index_buffer_memory_);
	first_time = false;
}

void ui_vk_draw(int _crrnt_frm, struct pipeline_vk *_ppln, bool frst_tm, VkDeviceSize vbffr_ffst, VkDeviceSize ibffr_ffst, vk_rsrs *_rsrs)
{
	vkCmdBindPipeline(vk_command_buffer_[_crrnt_frm], VK_PIPELINE_BIND_POINT_GRAPHICS, _ppln->vk_pipeline_);
	if (frst_tm)
	{
		VkViewport viewport = { 0 };
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = _rsrs->vk_swap_chain_image_extent_2d_.width;
		viewport.height = _rsrs->vk_swap_chain_image_extent_2d_.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(vk_command_buffer_[_crrnt_frm], 0, 1, &viewport);

		VkRect2D scissor = { 0 };
		scissor.offset = (VkOffset2D){ 0, 0 };
		scissor.extent = _rsrs->vk_swap_chain_image_extent_2d_;
		vkCmdSetScissor(vk_command_buffer_[_crrnt_frm], 0, 1, &scissor);
	}


	vkCmdPushConstants(
		vk_command_buffer_[_crrnt_frm],
		_ppln->vk_pipeline_layout_,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		0,
		sizeof(_ppln->pconstant),
		&_ppln->pconstant
	);

	vkCmdBindDescriptorSets(
		vk_command_buffer_[_crrnt_frm],
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		_ppln->vk_pipeline_layout_,
		0,
		1,
		&_ppln->vk_descriptor_sets_[_crrnt_frm],
		0,
		NULL
	);

	VkBuffer vertex_buffers_timeline[] = { _ppln->vk_vertex_buffer_ };
	VkDeviceSize offsets_timeline[] = { vbffr_ffst };
	vkCmdBindVertexBuffers(
		vk_command_buffer_[_crrnt_frm],
		0,
		1,
		vertex_buffers_timeline,
		offsets_timeline
	);

	vkCmdBindIndexBuffer(
		vk_command_buffer_[_crrnt_frm],
		_ppln->vk_index_buffer_,
		ibffr_ffst,
		VK_INDEX_TYPE_UINT32
	);
	vkCmdDrawIndexed(
		vk_command_buffer_[_crrnt_frm],
		_ppln->indices_count,
		1,
		0,
		0,
		0
	);

}


bool ui_draw_button(ui_label lbl, SDL_Window *_wndw)
{
	ui_label lb = { 0 };
	lbl.scale[0] *= 0.8;
	lb.st_typ = global_all_labels[global_all_labels_count].st_typ;
	global_all_labels[global_all_labels_count] = lbl;
	global_all_labels[global_all_labels_count].st_typ = lb.st_typ;
	global_all_labels_count++;
	switch (global_all_labels[global_all_labels_count - 1].st_typ)
	{
	case lbl_st_UNSELECTED:
		return false;
	case lbl_st_SELECTED:
		return true;
	case lbl_st_HOVERING:
		return false;
	}
}

void ui_events(SDL_Window *_wndw, SDL_Event *event)
{
	static uint32_t time = 0;
	int xpos, ypos;
	int width, height;
	uint32_t buttons = SDL_GetMouseState(&xpos, &ypos);
	SDL_GetWindowSize(_wndw, &width, &height);
	int Length;
	const Uint8 *KeyboardState = SDL_GetKeyboardState(&Length);

	for (int i = 0; i < global_all_labels_count; i++)
	{
		gn_txt(global_all_labels[i].text, strlen(global_all_labels[i].text), &global_base_font);
		mat4 model;
		glm_mat4_identity(model);
		glm_translate(model, (vec3) { global_all_labels[i].ps[0], -global_all_labels[i].ps[1], 0 }); /* Negative the position for a workaround*/
		glm_scale(model, global_all_labels[i].scale);
		glm_scale(model, (vec3) { global_base_font.max_w * 1.6, global_base_font.max_h * 1.7, 0 });

		float size = 0.5;
		vec4 b_l = { -1 * size, -1 * size, 0, 1 };
		vec4 t_l = { -1 * size, 1 * size, 0, 1 };
		vec4 t_r = { 1 * size, 1 * size, 0, 1 };
		vec4 b_r = { 1 * size, -1 * size, 0, 1 };
		glm_mat4_mulv(model, b_l, b_l);
		glm_mat4_mulv(model, t_r, t_r);

		float ms_x = 2 * (float) xpos / width - 1;
		float ms_y = -(2 * (float) ypos / height - 1);

		if ((ms_x > b_l[0]) && (ms_x < t_r[0]) && (ms_y > b_l[1]) && (ms_y < t_r[1]))
		{
			if ((buttons & SDL_BUTTON_LMASK) && !KeyboardState[SDL_SCANCODE_LALT])
			{
				global_all_labels[i].st_typ = lbl_st_SELECTED;
				continue;
			}
			global_all_labels[i].st_typ = lbl_st_HOVERING;
			continue;
		}
		global_all_labels[i].st_typ = lbl_st_UNSELECTED;
	}
}

void ui_update(int *_crrnt_frm)
{
	VkDeviceSize size;
	VkDeviceSize nsize;
	bool stt[2] = { true, false };
	for (int i = 0; i < global_all_labels_count; i++)
	{
		switch (global_all_labels[i].typ)
		{
		case BUTTON:
			ui_update_vbuffers(global_all_labels[i], &base_, i, *_crrnt_frm, true, &size, &nsize);
			break;
		}
	}
}

void ui_render(int *_crrnt_frm, vk_rsrs *_rsrs)
{
	VkDeviceSize size;
	VkDeviceSize nsize;
	bool stt[2] = { true, false };
	for (int i = 0; i < global_all_labels_count; i++)
	{
		// यो कस्तो दामी देखिएको
		switch (global_all_labels[i].typ)
		{
		case BUTTON:
			switch (i)
			{
			case 0:
				switch (global_all_labels[i].st_typ)
				{
				case lbl_st_HOVERING:
					glm_vec3_copy(global_all_labels[i].hvrd_clr, base_.ppln.pconstant.v1);
					break;
				case lbl_st_UNSELECTED:
					glm_vec3_copy(global_all_labels[i].nrml_clr, base_.ppln.pconstant.v1);
					break;
				case lbl_st_SELECTED:
					glm_vec3_copy(global_all_labels[i].slctd_clr, base_.ppln.pconstant.v1);
					break;
				default:
					break;
				}
				glm_vec3_copy(global_all_labels[i].txt_clr, base_.fnt.ppln.pconstant.v1);
				ui_get(global_all_labels[i], &base_, &size, &nsize, i);
				ui_vk_draw(*_crrnt_frm, &base_.ppln, true, 0, 0, _rsrs);
				ui_vk_draw(*_crrnt_frm, &base_.fnt.ppln, true, size, nsize, _rsrs);
				break;
			default:
				switch (global_all_labels[i].st_typ)
				{
				case lbl_st_HOVERING:
					glm_vec3_copy(global_all_labels[i].hvrd_clr, base_.ppln.pconstant.v1);
					break;
				case lbl_st_UNSELECTED:
					glm_vec3_copy(global_all_labels[i].nrml_clr, base_.ppln.pconstant.v1);
					break;
				case lbl_st_SELECTED:
					glm_vec3_copy(global_all_labels[i].slctd_clr, base_.ppln.pconstant.v1);
					break;
				default:
					break;
				}
				glm_vec3_copy(global_all_labels[i].txt_clr, base_.fnt.ppln.pconstant.v1);
				ui_get(global_all_labels[i], &base_, &size, &nsize, i);
				ui_vk_draw(*_crrnt_frm, &base_.ppln, false, 0, 0, _rsrs);
				ui_vk_draw(*_crrnt_frm, &base_.fnt.ppln, false, size, nsize, _rsrs);
				break;
			}
		}
	}
	global_all_labels_count = 0;
}

void ui_destroy(vk_rsrs *_rsrs)
{
	global_all_labels_count = 0;
	vkDestroyImageView(vk_logical_device_, base_.fnt.fnt_img_vw, NULL);
	vkDestroySampler(vk_logical_device_, base_.fnt.fnt_img_smplr, NULL);
	vkDestroyImage(vk_logical_device_, base_.fnt.fnt_img, NULL);
	vkFreeMemory(vk_logical_device_, base_.fnt.fnt_img_mmry, NULL);

	pipeline_vk_destroy(&base_.ppln);
	pipeline_vk_destroy(&base_.fnt.ppln);
	fnt_free();
}


int sdl_get_button_keyboard_number()
{
	int Length;
	const Uint8 *KeyboardState = SDL_GetKeyboardState(&Length);


	int numbers[] = {
		SDL_SCANCODE_0,
		SDL_SCANCODE_1,
		SDL_SCANCODE_2,
		SDL_SCANCODE_3,
		SDL_SCANCODE_4,
		SDL_SCANCODE_5,
		SDL_SCANCODE_6,
		SDL_SCANCODE_7,
		SDL_SCANCODE_8,
		SDL_SCANCODE_9,
		SDL_SCANCODE_BACKSPACE,
		SDL_SCANCODE_PERIOD,
		SDL_SCANCODE_MINUS
	};

	for (int i = 0; i < 13; i++)
	{
		if (KeyboardState[numbers[i]])
		{
			return i;
		}
	}

	return KSAI_INT32_MAX;
}
