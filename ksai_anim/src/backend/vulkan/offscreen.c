#include "offscreen.h"

void prepare_offscreen(vk_rsrs *_rsrs, renderer_backend *backend)
{
	VkFormat frmt = _rsrs->vk_swap_chain_image_format_;
	create_image_util(
		_rsrs->vk_swap_chain_image_extent_2d_.width,
		_rsrs->vk_swap_chain_image_extent_2d_.height,
		frmt,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&backend->mspk.img_clr_att_,
		&backend->mspk.img_mmry_clr_att_,
		vk_logical_device_
	);
	VkImageViewCreateInfo img_vw_cr = (VkImageViewCreateInfo){
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = frmt,
		.subresourceRange = (VkImageSubresourceRange) {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
		.image = backend->mspk.img_clr_att_
	};
	if (vkCreateImageView(vk_logical_device_, &img_vw_cr, NULL, &backend->mspk.img_vw_clr_att_) != VK_SUCCESS)
		printf("Failed To create Image view");

	VkSamplerCreateInfo smplr = (VkSamplerCreateInfo){
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.mipLodBias = 0.0f,
		.maxAnisotropy = 1.0f,
		.minLod = 0.0f,
		.maxLod = 1.0f,
		.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE
	};
	if (vkCreateSampler(vk_logical_device_, &smplr, NULL, &backend->mspk.smplr_) != VK_SUCCESS)
		printf("Failed to Create Sampler");


	VkFormat format = _rsrs->vk_depth_image_format;
	create_image_util(
		_rsrs->vk_swap_chain_image_extent_2d_.width,
		_rsrs->vk_swap_chain_image_extent_2d_.height,
		format,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&backend->mspk.img_dpth_att_,
		&backend->mspk.img_mmry_dpth_att_, vk_logical_device_
	);
	backend->mspk.img_vw_dpth_att_ = create_image_view_util2(backend->mspk.img_dpth_att_, format, VK_IMAGE_ASPECT_DEPTH_BIT);
	transition_image_layout_util(
		backend->mspk.img_dpth_att_,
		format,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		vk_command_pool_,
		_rsrs->vk_graphics_queue_
	);


	/* Create Another Render Pass */
	VkAttachmentDescription depth_attachment = (VkAttachmentDescription){
		.format = _rsrs->vk_depth_image_format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference depth_attachment_ref = (VkAttachmentReference)
	{
		.attachment = 1,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};
	VkAttachmentDescription clr_att_desp = (VkAttachmentDescription)
	{
		.format = frmt,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	};
	VkAttachmentReference clr_att_ref = (VkAttachmentReference)
	{
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};
	VkSubpassDescription sbpss_dsp = (VkSubpassDescription)
	{
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &clr_att_ref,
		.pDepthStencilAttachment = &depth_attachment_ref
	};

	VkSubpassDependency sbpss_deps[3];
	sbpss_deps[0] = (VkSubpassDependency)
	{
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		.srcAccessMask = 0,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		.dependencyFlags = 0
	};
	sbpss_deps[1] = (VkSubpassDependency)
	{
		.srcSubpass = 0,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		.srcAccessMask = 0,
		.dstStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		.dstAccessMask = 0,
		.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
	};

	VkAttachmentDescription attch_des[2] = { clr_att_desp, depth_attachment };

	VkRenderPassCreateInfo crt_rndr_pss = (VkRenderPassCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 2,
		.pAttachments = attch_des,
		.subpassCount = 1,
		.pSubpasses = &sbpss_dsp,
		.dependencyCount = 2,
		.pDependencies = sbpss_deps
	};

	if (vkCreateRenderPass(vk_logical_device_, &crt_rndr_pss, NULL, &backend->mspk.rndr_pss_) != VK_SUCCESS)
		printf("Failed to create Render pass");


	VkImageView views[2] = { backend->mspk.img_vw_clr_att_, backend->mspk.img_vw_dpth_att_ };
	VkFramebufferCreateInfo fbfr_crt = (VkFramebufferCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.renderPass = backend->mspk.rndr_pss_,
		.attachmentCount = 2,
		.pAttachments = views,
		.width = _rsrs->vk_swap_chain_image_extent_2d_.width,
		.height = _rsrs->vk_swap_chain_image_extent_2d_.height,
		.layers = 1
	};

	if (vkCreateFramebuffer(vk_logical_device_, &fbfr_crt, NULL, &backend->mspk.frm_bfr_) != VK_SUCCESS)
		printf("Failed to create Frame Buffer");

	backend->mspk.dscrptr_.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	backend->mspk.dscrptr_.imageView = backend->mspk.img_vw_clr_att_;
	backend->mspk.dscrptr_.sampler = backend->mspk.smplr_;

	VkDeviceSize siz_e = (uint64_t) _rsrs->vk_swap_chain_image_extent_2d_.width * _rsrs->vk_swap_chain_image_extent_2d_.height * 4;

	create_buffer_util(siz_e,
		VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		&backend->mspk.bfr_,
		&backend->mspk.bfr_mmry_,
		vk_logical_device_
	);
	vkMapMemory(vk_logical_device_, backend->mspk.bfr_mmry_, 0, siz_e, 0, &backend->mspk.data_);

	create_buffer_util(siz_e,
		VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		&backend->mspk.render_buffer,
		&backend->mspk.render_buffer_memory,
		vk_logical_device_
	);
	vkMapMemory(vk_logical_device_, backend->mspk.render_buffer_memory, 0, siz_e, 0, &backend->mspk.render_buffer_data);


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

	VkDescriptorSetLayoutBinding bindings[KSAI_VK_DESCRIPTOR_POOL_SIZE] = {
		ubo_layout_binding,
		sampler_layout_binding,
		(VkDescriptorSetLayoutBinding){
			.binding = 2,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
			.pImmutableSamplers = NULL
		},
		(VkDescriptorSetLayoutBinding){
			.binding = 3,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
			.pImmutableSamplers = NULL
		},
		(VkDescriptorSetLayoutBinding){
			.binding = 4,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
			.pImmutableSamplers = NULL
		}
	};


	VkVertexInputBindingDescription binding_desp = (VkVertexInputBindingDescription){
		.binding = 0,
		.stride = sizeof(kie_Vertex),
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};
	VkVertexInputAttributeDescription attr_desp[6];

	attr_desp[0] = (VkVertexInputAttributeDescription){
		.binding = 0,
		.location = 0,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(kie_Vertex, position)
	};

	attr_desp[1] = (VkVertexInputAttributeDescription){
		.binding = 0,
		.location = 1,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(kie_Vertex, normal)
	};

	attr_desp[2] = (VkVertexInputAttributeDescription){
		.binding = 0,
		.location = 2,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(kie_Vertex, color)
	};

	attr_desp[3] = (VkVertexInputAttributeDescription){
		.binding = 0,
		.location = 3,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(kie_Vertex, tangent)
	};

	attr_desp[4] = (VkVertexInputAttributeDescription){
		.binding = 0,
		.location = 4,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(kie_Vertex, bit_tangent)
	};

	attr_desp[5] = (VkVertexInputAttributeDescription){
		.binding = 0,
		.location = 5,
		.format = VK_FORMAT_R32G32_SFLOAT,
		.offset = offsetof(kie_Vertex, tex_coord)
	};


	VkDescriptorPoolSize pool_sizes[5] = { 0 };
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_sizes[0].descriptorCount = MAX_FRAMES_IN_FLIGHT;

	pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_sizes[1].descriptorCount = MAX_FRAMES_IN_FLIGHT;

	pool_sizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_sizes[2].descriptorCount = MAX_FRAMES_IN_FLIGHT;

	pool_sizes[3].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_sizes[3].descriptorCount = MAX_FRAMES_IN_FLIGHT;

	pool_sizes[4].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_sizes[4].descriptorCount = MAX_FRAMES_IN_FLIGHT;


	create_vulkan_pipeline3(
		_rsrs,
		&backend->constant_color,
		KSAI_VK_DESCRIPTOR_POOL_SIZE,
		bindings,
		"res/shaders/renderer/constant/vshader.spv",
		"res/shaders/renderer/constant/fshader.spv",
		&binding_desp,
		1,
		attr_desp,
		6,
		KSAI_VK_DESCRIPTOR_POOL_SIZE,
		backend->pool_sizes
	);
}

void destroy_offscreen(vk_rsrs *rsrs, renderer_backend *backend)
{
	pipeline_vk_destroy3(&backend->constant_color);

	vkUnmapMemory(vk_logical_device_, backend->mspk.render_buffer_memory);
	vkDestroyBuffer(vk_logical_device_, backend->mspk.render_buffer, NULL);
	vkFreeMemory(vk_logical_device_, backend->mspk.render_buffer_memory, NULL);

	vkUnmapMemory(vk_logical_device_, backend->mspk.bfr_mmry_);
	vkDestroySampler(vk_logical_device_, backend->mspk.smplr_, NULL);
	vkDestroyBuffer(vk_logical_device_, backend->mspk.bfr_, NULL);
	vkFreeMemory(vk_logical_device_, backend->mspk.bfr_mmry_, NULL);

	vkDestroyImage(vk_logical_device_, backend->mspk.img_clr_att_, NULL);
	vkFreeMemory(vk_logical_device_, backend->mspk.img_mmry_clr_att_, NULL);
	vkDestroyImageView(vk_logical_device_, backend->mspk.img_vw_clr_att_, NULL);


	vkDestroyImage(vk_logical_device_, backend->mspk.img_dpth_att_, NULL);
	vkFreeMemory(vk_logical_device_, backend->mspk.img_mmry_dpth_att_, NULL);
	vkDestroyImageView(vk_logical_device_, backend->mspk.img_vw_dpth_att_, NULL);

	vkDestroyFramebuffer(vk_logical_device_, backend->mspk.frm_bfr_, NULL);
	vkDestroyRenderPass(vk_logical_device_, backend->mspk.rndr_pss_, NULL);
}

void recreate_offscreen(vk_rsrs *_rsrs, renderer_backend *backend)
{	
	vkUnmapMemory(vk_logical_device_, backend->mspk.render_buffer_memory);
	vkDestroyBuffer(vk_logical_device_, backend->mspk.render_buffer, NULL);
	vkFreeMemory(vk_logical_device_, backend->mspk.render_buffer_memory, NULL);


	vkUnmapMemory(vk_logical_device_, backend->mspk.bfr_mmry_);
	vkDestroySampler(vk_logical_device_, backend->mspk.smplr_, NULL);
	vkDestroyBuffer(vk_logical_device_, backend->mspk.bfr_, NULL);
	vkFreeMemory(vk_logical_device_, backend->mspk.bfr_mmry_, NULL);


	vkDestroyImage(vk_logical_device_, backend->mspk.img_clr_att_, NULL);
	vkFreeMemory(vk_logical_device_, backend->mspk.img_mmry_clr_att_, NULL);
	vkDestroyImageView(vk_logical_device_, backend->mspk.img_vw_clr_att_, NULL);


	vkDestroyImage(vk_logical_device_, backend->mspk.img_dpth_att_, NULL);
	vkFreeMemory(vk_logical_device_, backend->mspk.img_mmry_dpth_att_, NULL);
	vkDestroyImageView(vk_logical_device_, backend->mspk.img_vw_dpth_att_, NULL);

	vkDestroyFramebuffer(vk_logical_device_, backend->mspk.frm_bfr_, NULL);
	vkDestroyRenderPass(vk_logical_device_, backend->mspk.rndr_pss_, NULL);


	VkFormat frmt = _rsrs->vk_swap_chain_image_format_;
	create_image_util(
		_rsrs->vk_swap_chain_image_extent_2d_.width,
		_rsrs->vk_swap_chain_image_extent_2d_.height,
		frmt,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&backend->mspk.img_clr_att_,
		&backend->mspk.img_mmry_clr_att_,
		vk_logical_device_
	);
	VkImageViewCreateInfo img_vw_cr = (VkImageViewCreateInfo){
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = frmt,
		.subresourceRange = (VkImageSubresourceRange) {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
		.image = backend->mspk.img_clr_att_
	};
	if (vkCreateImageView(vk_logical_device_, &img_vw_cr, NULL, &backend->mspk.img_vw_clr_att_) != VK_SUCCESS)
		printf("Failed To create Image view");

	VkSamplerCreateInfo smplr = (VkSamplerCreateInfo){
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.mipLodBias = 0.0f,
		.maxAnisotropy = 1.0f,
		.minLod = 0.0f,
		.maxLod = 1.0f,
		.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE
	};
	if (vkCreateSampler(vk_logical_device_, &smplr, NULL, &backend->mspk.smplr_) != VK_SUCCESS)
		printf("Failed to Create Sampler");


	VkFormat format = _rsrs->vk_depth_image_format;
	create_image_util(
		_rsrs->vk_swap_chain_image_extent_2d_.width,
		_rsrs->vk_swap_chain_image_extent_2d_.height,
		format,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&backend->mspk.img_dpth_att_,
		&backend->mspk.img_mmry_dpth_att_, vk_logical_device_
	);
	backend->mspk.img_vw_dpth_att_ = create_image_view_util2(backend->mspk.img_dpth_att_, format, VK_IMAGE_ASPECT_DEPTH_BIT);
	transition_image_layout_util(
		backend->mspk.img_dpth_att_,
		format,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		vk_command_pool_,
		_rsrs->vk_graphics_queue_
	);


	/* Create Another Render Pass */
	VkAttachmentDescription depth_attachment = (VkAttachmentDescription){
		.format = _rsrs->vk_depth_image_format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference depth_attachment_ref = (VkAttachmentReference)
	{
		.attachment = 1,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};
	VkAttachmentDescription clr_att_desp = (VkAttachmentDescription)
	{
		.format = frmt,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	};
	VkAttachmentReference clr_att_ref = (VkAttachmentReference)
	{
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};
	VkSubpassDescription sbpss_dsp = (VkSubpassDescription)
	{
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &clr_att_ref,
		.pDepthStencilAttachment = &depth_attachment_ref
	};

	VkSubpassDependency sbpss_deps[2];
	sbpss_deps[0] = (VkSubpassDependency)
	{
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		.srcAccessMask = 0,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		.dependencyFlags = 0
	};
	sbpss_deps[1] = (VkSubpassDependency)
	{
		.srcSubpass = 0,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		.srcAccessMask = 0,
		.dstStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		.dstAccessMask = 0,
		.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT

	};

	VkAttachmentDescription attch_des[2] = { clr_att_desp, depth_attachment };

	VkRenderPassCreateInfo crt_rndr_pss = (VkRenderPassCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 2,
		.pAttachments = attch_des,
		.subpassCount = 1,
		.pSubpasses = &sbpss_dsp,
		.dependencyCount = 2,
		.pDependencies = sbpss_deps
	};

	if (vkCreateRenderPass(vk_logical_device_, &crt_rndr_pss, NULL, &backend->mspk.rndr_pss_) != VK_SUCCESS)
		printf("Failed to create Render pass");


	VkImageView views[2] = { backend->mspk.img_vw_clr_att_, backend->mspk.img_vw_dpth_att_ };
	VkFramebufferCreateInfo fbfr_crt = (VkFramebufferCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.renderPass = backend->mspk.rndr_pss_,
		.attachmentCount = 2,
		.pAttachments = views,
		.width = _rsrs->vk_swap_chain_image_extent_2d_.width,
		.height = _rsrs->vk_swap_chain_image_extent_2d_.height,
		.layers = 1
	};

	if (vkCreateFramebuffer(vk_logical_device_, &fbfr_crt, NULL, &backend->mspk.frm_bfr_) != VK_SUCCESS)
		printf("Failed to create Frame Buffer");

	VkDeviceSize siz_e = (uint64_t) _rsrs->vk_swap_chain_image_extent_2d_.width * _rsrs->vk_swap_chain_image_extent_2d_.height * 4;
	create_buffer_util(siz_e,
		VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		&backend->mspk.bfr_,
		&backend->mspk.bfr_mmry_,
		vk_logical_device_
	);
	vkMapMemory(vk_logical_device_, backend->mspk.bfr_mmry_, 0, siz_e, 0, &backend->mspk.data_);

	create_buffer_util(siz_e,
		VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		&backend->mspk.render_buffer,
		&backend->mspk.render_buffer_memory,
		vk_logical_device_
	);
	vkMapMemory(vk_logical_device_, backend->mspk.render_buffer_memory, 0, siz_e, 0, &backend->mspk.render_buffer_data);

}

void render_offscreen_begin(vk_rsrs *rsrs, renderer_backend *backend)
{
	render_offscreen_begin_buf(rsrs, backend, vk_command_buffer_[rsrs->current_frame], (vec3) {0, 0, 0});
}

void render_offscreen_end(vk_rsrs *rsrs, renderer_backend *backend)
{
	render_offscreen_end_buf(rsrs, backend, vk_command_buffer_[rsrs->current_frame]);
}

void render_offscreen_begin_buf(vk_rsrs *rsrs, renderer_backend *backend, VkCommandBuffer buffer, vec3 color)
{
	VkClearValue clear_color[2] = {
	(VkClearValue)
		{
		.color.float32[0] = color[0],
		.color.float32[1] = color[1],
		.color.float32[2] = color[2]
		},

		(VkClearValue)
		{
		.depthStencil = {1.0f, 0.0f}
		}
	};
	VkRenderPassBeginInfo bgn_rndrpss = (VkRenderPassBeginInfo)
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.clearValueCount = 2,
		.framebuffer = backend->mspk.frm_bfr_,
		.pNext = NULL,
		.renderArea = {0},
		.renderPass = backend->mspk.rndr_pss_,
		.pClearValues = clear_color,
		.renderArea = (VkRect2D) {
			.extent = rsrs->vk_swap_chain_image_extent_2d_,
			.offset = (VkOffset2D) {.x = 0, .y = 0}
		}
	};

	vkCmdBeginRenderPass(buffer, &bgn_rndrpss, VK_SUBPASS_CONTENTS_INLINE);
}

void render_offscreen_end_buf(vk_rsrs *rsrs, renderer_backend *backend, VkCommandBuffer buffer)
{
	vkCmdEndRenderPass(buffer);
}
