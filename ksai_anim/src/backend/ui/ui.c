#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include <stdio.h>
#include <stdbool.h>
#include "ui.h"
#include "font.h"


static lu_lbl_bs base_;
static lu_lbl lbls_ll_[MAX_BUFFER_SIZE];
static uint32_t lbls_ll_cnt_ = 0;
static fnt_d bs_fnt_;

void lu_null_lbls_cnt()
{
	lbls_ll_cnt_ = 0;
}


void lu_int(int sz_fctr, vk_rsrs *_rsrs)
{

	static bool first_time = true;
	if (first_time == true)
	{

		lu_lbl label;
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
		prs_bm_fnt("res/font/font.fnt", &bs_fnt_);

		extui_create_vulkan_pipeline(
			_rsrs,
			&base_.ppln,
			1,
			bindings,
			"res/shaders/button/vshader.spv",
			"res/shaders/button/fshader.spv",
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


		strcpy(base_.fnt.fnt_img_pth, "res/font/fnt.png");
		char *d[1 << 8] = { base_.fnt.fnt_img_pth };
		gn_txt(label.text, strlen(label.text), &base_.fnt);
		extui_create_vulkan_pipeline(
			_rsrs,
			&base_.fnt.ppln,
			2,
			bindings,
			"res/shaders/text/vshader.spv",
			"res/shaders/text/fshader.spv",
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

void lu_gt(lu_lbl lbl, lu_lbl_bs *_bttn, VkDeviceSize *_sz, VkDeviceSize *_nsz, int i)
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
		glm_scale(model, (vec3) { _bttn->fnt.max_w / 2, _bttn->fnt.max_h, 0 });
		glm_scale(model, (vec3) { 2.5, 2, 0 });
		glm_mat4_copy(model, _bttn->ppln.pconstant.mvp);
	}
}

void lu_bttn_uvbffr(lu_lbl lbl, lu_lbl_bs *_bttn, int i, int _crrnt_frm, bool map_it, VkDeviceSize *_sz, VkDeviceSize *_nsz
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


void lu_vlkn_drw(int _crrnt_frm, struct l_vlkn_ppln *_ppln, bool frst_tm, VkDeviceSize vbffr_ffst, VkDeviceSize ibffr_ffst, vk_rsrs *_rsrs)
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


/* For Plugins */
bool lu_drw_lbl(lu_lbl lbl, SDL_Window *_wndw, SDL_Event *event)
{
	int xpos, ypos;
	int width, height;
	SDL_GetMouseState(&xpos, &ypos);
	SDL_GetWindowSize(_wndw, &width, &height);

	gn_txt(lbl.text, strlen(lbl.text), &bs_fnt_);
	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, (vec3) { lbl.ps[0], -lbl.ps[1], 0 }); /* Negative the position for a workaround*/
	glm_scale(model, lbl.scale);
	glm_scale(model, (vec3) { bs_fnt_.max_w / 2, bs_fnt_.max_h, 0 });
	glm_scale(model, (vec3) { 2.5, 2, 0 });
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
		
		if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
		{
			lbls_ll_[lbls_ll_cnt_] = lbl;
			lbls_ll_[lbls_ll_cnt_].st_typ = lbl_st_SELECTED;
			lbls_ll_cnt_++;
			return true;
		}
		lbls_ll_[lbls_ll_cnt_] = lbl;
		lbls_ll_[lbls_ll_cnt_].st_typ = lbl_st_HOVERING;
		lbls_ll_cnt_++;
		return false;
	}
	lbls_ll_[lbls_ll_cnt_] = lbl;
	lbls_ll_[lbls_ll_cnt_].st_typ = lbl_st_UNSELECTED;
	lbls_ll_cnt_++;
	return false;
}

void lu_updt(int *_crrnt_frm)
{
	VkDeviceSize size;
	VkDeviceSize nsize;
	bool stt[2] = { true, false };
	for (int i = 0; i < lbls_ll_cnt_; i++)
	{
		switch (lbls_ll_[i].typ)
		{
		case BUTTON:
			lu_bttn_uvbffr(lbls_ll_[i], &base_, i, *_crrnt_frm, true, &size, &nsize);
			break;
		}
	}
}

void lu_rndr(int *_crrnt_frm, vk_rsrs *_rsrs)
{
	VkDeviceSize size;
	VkDeviceSize nsize;
	bool stt[2] = { true, false };
	for (int i = 0; i < lbls_ll_cnt_; i++)
	{
		// यो कस्तो दामी देखिएको
		switch (lbls_ll_[i].typ)
		{
		case BUTTON:
			switch (i)
			{
			case 0:
				switch (lbls_ll_[i].st_typ)
				{
				case lbl_st_HOVERING:
					glm_vec3_copy(lbls_ll_[i].hvrd_clr, base_.ppln.pconstant.v1);
					break;
				case lbl_st_UNSELECTED:
					glm_vec3_copy(lbls_ll_[i].nrml_clr, base_.ppln.pconstant.v1);
					break;
				case lbl_st_SELECTED:
					glm_vec3_copy(lbls_ll_[i].slctd_clr, base_.ppln.pconstant.v1);
					break;
				default:
					break;
				}
				lu_gt(lbls_ll_[i], &base_, &size, &nsize, i);
				lu_vlkn_drw(*_crrnt_frm, &base_.ppln, true, 0, 0, _rsrs);
				lu_vlkn_drw(*_crrnt_frm, &base_.fnt.ppln, true, size, nsize, _rsrs);
				break;
			default:
				switch (lbls_ll_[i].st_typ)
				{
				case lbl_st_HOVERING:
					glm_vec3_copy(lbls_ll_[i].hvrd_clr, base_.ppln.pconstant.v1);
					break;
				case lbl_st_UNSELECTED:
					glm_vec3_copy(lbls_ll_[i].nrml_clr, base_.ppln.pconstant.v1);
					break;
				case lbl_st_SELECTED:
					glm_vec3_copy(lbls_ll_[i].slctd_clr, base_.ppln.pconstant.v1);
					break;
				default:
					break;
				}
				lu_gt(lbls_ll_[i], &base_, &size, &nsize, i);
				lu_vlkn_drw(*_crrnt_frm, &base_.ppln, false, 0, 0, _rsrs);
				lu_vlkn_drw(*_crrnt_frm, &base_.fnt.ppln, false, size, nsize, _rsrs);
				break;
			}
		}
	}
	lbls_ll_cnt_ = 0;
}

