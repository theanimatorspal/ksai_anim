#include "advanced.h"

void prepare_skybox(vk_rsrs *rsrs, renderer_backend *backend)
{
	/* SKYBOX RENDER */
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


	VkDescriptorPoolSize pool_sizes[2] = { 0 };
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_sizes[0].descriptorCount = MAX_FRAMES_IN_FLIGHT;
	pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_sizes[1].descriptorCount = MAX_FRAMES_IN_FLIGHT;

	backend->pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	backend->pool_sizes[0].descriptorCount = MAX_FRAMES_IN_FLIGHT;

	backend->pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	backend->pool_sizes[1].descriptorCount = MAX_FRAMES_IN_FLIGHT;

	backend->pool_sizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	backend->pool_sizes[2].descriptorCount = MAX_FRAMES_IN_FLIGHT;

	backend->pool_sizes[3].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	backend->pool_sizes[3].descriptorCount = MAX_FRAMES_IN_FLIGHT;

	backend->pool_sizes[4].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	backend->pool_sizes[4].descriptorCount = MAX_FRAMES_IN_FLIGHT;

	create_vulkan_pipeline3(
		rsrs,
		&backend->skybox,
		2,
		bindings,
		"res/shaders/renderer/skybox/vshader.spv",
		"res/shaders/renderer/skybox/fshader.spv",
		&binding_desp,
		1,
		attr_desp,
		6,
		2,
		pool_sizes
	);

	int tex_width, tex_height, tex_channels;
	stbi_set_flip_vertically_on_load(true);
	stbi_uc *pixels = stbi_load("res/textures/px.png", &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
	VkDeviceSize image_size = tex_width * tex_height * 4;
	if (!pixels)
		printf("error, cannot load image_temporary_testing_ \n");
	stbi_image_free(pixels);

	char *paths[] = {
		"res/textures/px.png",
		"res/textures/nx.png",
		"res/textures/py.png",
		"res/textures/ny.png",
		"res/textures/pz.png",
		"res/textures/nz.png",
	};

	create_image_util_array_cube(
		VK_IMAGE_TYPE_2D,
		tex_width, tex_height,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&backend->skybox_image,
		&backend->skybox_image_memory,
		vk_logical_device_,
		6
	);
	create_texture_sampler_skybox(
		&backend->skybox_sampler,
		vk_logical_device_,
		vk_physical_device_
	);

	backend->skybox_image_view = create_image_view_util2_skybox(
		backend->skybox_image,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_ASPECT_COLOR_BIT
	);

	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;
	create_buffer_util(
		6 * image_size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&staging_buffer,
		&staging_buffer_memory,
		vk_logical_device_
	);


	void *data;
	vkMapMemory(vk_logical_device_, staging_buffer_memory, 0, image_size, 0, &data);
	for (int i = 0; i < 6; i++)
	{
		int tex_width, tex_height, tex_channels;
		stbi_uc *pixels = stbi_load(paths[i], &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
		VkDeviceSize image_size = tex_width * tex_height * 4;
		if (!pixels)
			printf("error, cannot load image_temporary_testing_ \n");
		memcpy(data, pixels, image_size);
		stbi_image_free(pixels);
	}
	vkUnmapMemory(vk_logical_device_, staging_buffer_memory);
	transition_image_layout_util_layered(
		6,
		backend->skybox_image,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		vk_command_pool_,
		rsrs->vk_graphics_queue_
	);
	copy_buffer_to_image_util_layered(
		6,
		staging_buffer,
		backend->skybox_image,
		tex_width,
		tex_height,
		vk_command_pool_,
		rsrs->vk_graphics_queue_
	);
	transition_image_layout_util_layered(
		6,
		backend->skybox_image,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		vk_command_pool_,
		rsrs->vk_graphics_queue_
	);
	vkDeviceWaitIdle(vk_logical_device_);
	vkDestroyBuffer(vk_logical_device_, staging_buffer, NULL);
	vkFreeMemory(vk_logical_device_, staging_buffer_memory, NULL);
}

void destroy_skybox(vk_rsrs *rsrs, renderer_backend *backend)
{
	vkDestroyImageView(vk_logical_device_, backend->skybox_image_view, NULL);
	vkDestroySampler(vk_logical_device_, backend->skybox_sampler, NULL);
	vkDestroyImage(vk_logical_device_, backend->skybox_image, NULL);
	vkFreeMemory(vk_logical_device_, backend->skybox_image_memory, NULL);
	pipeline_vk_destroy3(&backend->skybox);
}

void draw_skybox_backend(vk_rsrs *rsrs, renderer_backend *backend, kie_Scene *scene)
{
		vkCmdSetDepthTestEnable(vk_command_buffer_[rsrs->current_frame], VK_TRUE);
		kie_Object *the_mesh = NULL;
		vkCmdBindPipeline(vk_command_buffer_[rsrs->current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, backend->skybox.vk_pipeline_);

		vkCmdBindDescriptorSets(
			vk_command_buffer_[rsrs->current_frame],
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			backend->checker_pipeline.vk_pipeline_layout_,
			0,
			1,
			&backend->descriptor_sets[0][rsrs->current_frame],
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

		vkCmdSetViewport(vk_command_buffer_[rsrs->current_frame], 0, 1, &viewport);

		VkRect2D scissor = { 0 };
		scissor.offset = (VkOffset2D){ 0, 0 };
		scissor.extent = rsrs->vk_swap_chain_image_extent_2d_;
		vkCmdSetScissor(vk_command_buffer_[rsrs->current_frame], 0, 1, &scissor);

		VkBuffer vertex_buffers[] = { backend->vbuffer };
		VkDeviceSize offsets[] = { backend->voffsets[0]};
		vkCmdBindVertexBuffers(vk_command_buffer_[rsrs->current_frame], 0, 1, vertex_buffers, offsets);

		vkCmdBindIndexBuffer(vk_command_buffer_[rsrs->current_frame], backend->ibuffer, backend->ioffsets[0], VK_INDEX_TYPE_UINT32);


		vkCmdPushConstants(vk_command_buffer_[rsrs->current_frame], backend->checker_pipeline.vk_pipeline_layout_, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(backend->checker_pipeline.pconstant), &backend->checker_pipeline.pconstant);
		vkCmdDrawIndexed(vk_command_buffer_[rsrs->current_frame], the_mesh->indices_count, 1, 0, 0, 0);
		vkCmdSetDepthTestEnable(vk_command_buffer_[rsrs->current_frame], VK_FALSE);

}