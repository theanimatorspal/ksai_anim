#include "advanced.h"

void prepare_skybox(vk_rsrs *rsrs, renderer_backend *backend)
{

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
		(VkDescriptorSetLayoutBinding)
{
.binding = 2,
.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
.descriptorCount = 1,
.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
.pImmutableSamplers = NULL
},

(VkDescriptorSetLayoutBinding)
{
.binding = 3,
.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
.descriptorCount = 1,
.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
.pImmutableSamplers = NULL
},

(VkDescriptorSetLayoutBinding)
{
.binding = 4,
.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
.descriptorCount = 1,
.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
.pImmutableSamplers = NULL
},
	};

	/* SKYBOX RENDER */

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

	create_vulkan_pipeline3_skybox(
		rsrs,
		&backend->skybox,
		KSAI_VK_DESCRIPTOR_POOL_SIZE,
		bindings,
		"res/shaders/renderer/skybox/vshader.spv",
		"res/shaders/renderer/skybox/fshader.spv",
		&binding_desp,
		1,
		attr_desp,
		6,
		KSAI_VK_DESCRIPTOR_POOL_SIZE,
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
		"res/textures/ny.png",
		"res/textures/py.png",
		"res/textures/pz.png",
		"res/textures/nz.png",
	};

	create_image_util_array_cube(
		VK_IMAGE_TYPE_2D,
		tex_width, tex_height,
		VK_FORMAT_R8G8B8A8_UNORM,
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
		VK_FORMAT_R8G8B8A8_UNORM,
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
		memcpy(data + i * image_size, pixels, image_size);
		stbi_image_free(pixels);
	}
	vkUnmapMemory(vk_logical_device_, staging_buffer_memory);
	transition_image_layout_util_layered(
		6,
		backend->skybox_image,
		VK_FORMAT_R8G8B8A8_UNORM,
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
		VK_FORMAT_R8G8B8A8_UNORM,
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

void draw_skybox_backend(vk_rsrs *rsrs, renderer_backend *backend, kie_Scene *scene, uint32_t skybox_obj_index)
{
	draw_skybox_backendbuf(rsrs, backend, scene, skybox_obj_index, vk_command_buffer_[rsrs->current_frame]);
}

void draw_skybox_backendbuf(vk_rsrs *rsrs, renderer_backend *backend, kie_Scene *scene, uint32_t skybox_obj_index, VkCommandBuffer cmd_buffer)
{
	int x = 3; /* SkyBox index */
	vkCmdSetDepthTestEnable(cmd_buffer, VK_FALSE);
	kie_Object *the_mesh = &scene->objects[skybox_obj_index];
	vkCmdBindPipeline(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, backend->skybox.vk_pipeline_);

	vkCmdBindDescriptorSets(
		cmd_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		backend->skybox.vk_pipeline_layout_,
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
	VkDeviceSize offsets[] = { backend->voffsets[skybox_obj_index] };
	vkCmdBindVertexBuffers(cmd_buffer, 0, 1, vertex_buffers, offsets);

	vkCmdBindIndexBuffer(cmd_buffer, backend->ibuffer, backend->ioffsets[skybox_obj_index], VK_INDEX_TYPE_UINT32);

	vkCmdPushConstants(cmd_buffer, backend->skybox.vk_pipeline_layout_, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(backend->skybox.pconstant), &backend->skybox.pconstant);
	vkCmdDrawIndexed(cmd_buffer, the_mesh->indices_count, 1, 0, 0, 0);
	//vkCmdSetDepthTestEnable(cmd_buffer, VK_FALSE);

}

typedef struct Particle1
{
	alignas(16) vec3 position;
	alignas(16) vec3 velocity;
	alignas(16) vec4 color;
} Particle1;

void prepare_particles(vk_rsrs *rsrs, renderer_backend *backend)
{
	VkDeviceSize buffer_size = MAX_NO_OF_PARTICLES * sizeof(struct Particle1);
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		create_buffer_util(
			buffer_size,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&backend->particle_system.buffer[i],
			&backend->particle_system.memory[i],
			vk_logical_device_
		);
		vkMapMemory(vk_logical_device_, backend->particle_system.memory[i], 0, buffer_size, 0, &backend->particle_system.buffer_data[i]);
		create_buffer_util(
			sizeof(uniforms),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&backend->particle_system.ubuffer[i],
			&backend->particle_system.ubuffer_memory[i],
			vk_logical_device_
		);
		vkMapMemory(vk_logical_device_, backend->particle_system.ubuffer_memory[i], 0, sizeof(uniforms), 0, &backend->particle_system.ubffer_data[i]);
	}


	VkDescriptorSetLayoutBinding ubo_layout_binding = { 0 };
	ubo_layout_binding = (VkDescriptorSetLayoutBinding){
		.binding = 0,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
		.pImmutableSamplers = NULL
	};

	VkDescriptorSetLayoutBinding sampler_layout_binding = { 0 };
	sampler_layout_binding = (VkDescriptorSetLayoutBinding){
		.binding = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
		.pImmutableSamplers = NULL
	};

	VkDescriptorSetLayoutBinding bindings[KSAI_VK_DESCRIPTOR_POOL_SIZE] = {
		ubo_layout_binding,
		sampler_layout_binding,
		(VkDescriptorSetLayoutBinding)
		{
		.binding = 2,
		.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT,
		.pImmutableSamplers = NULL
		}
	};

	VkVertexInputBindingDescription binding_desp = (VkVertexInputBindingDescription){
		.binding = 0,
		.stride = sizeof(Particle1),
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};
	VkVertexInputAttributeDescription attr_desp[2];

	attr_desp[0] = (VkVertexInputAttributeDescription){
		.binding = 0,
		.location = 0,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(Particle1, position)
	};

	attr_desp[1] = (VkVertexInputAttributeDescription){
		.binding = 0,
		.location = 1,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(Particle1, color)
	};


	VkDescriptorPoolSize pool_sizes[3] = { 0 };
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_sizes[0].descriptorCount = MAX_FRAMES_IN_FLIGHT;

	pool_sizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	pool_sizes[1].descriptorCount = MAX_FRAMES_IN_FLIGHT;

	pool_sizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	pool_sizes[2].descriptorCount = MAX_FRAMES_IN_FLIGHT;


	create_vulkan_pipeline3_compute(
		rsrs,
		&backend->particle_system.particle_pipe,
		3,
		bindings,
		"res/shaders/renderer/particles/vshader.spv",
		"res/shaders/renderer/particles/fshader.spv",
		"res/shaders/renderer/particles/cshader.spv",
		&binding_desp,
		1,
		attr_desp,
		2
	);

	float height = 0.5;
	float width = 1;
	for (int i = 0; i < MAX_NO_OF_PARTICLES; i++)
	{
		float r = 0.25f * sqrt(rand() / (float) RAND_MAX);
		float theta = rand() / (float) RAND_MAX * 2 * GLM_PI;
		float x = r * cos(theta) * height / width;
		float y = r * sin(theta);
		vec3 pos = { x, y, 0 };
		glm_vec3_copy(pos, backend->particle_system.particle[i].position);
		glm_normalize(pos);
		vec3 vel;
		glm_vec3_copy(pos, vel);
		float sm = 0.00025f;
		vel[0] *= sm;
		vel[1] *= sm;
		vel[2] *= sm;
		glm_vec3_copy(vel, backend->particle_system.particle[i].velocity);
		glm_vec4_copy((vec4) { rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, 1 }, backend->particle_system.particle[i].color);
	}

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		memcpy_s(backend->particle_system.buffer_data[i], buffer_size, backend->particle_system.particle, buffer_size);

		VkResult res = vkCreateDescriptorPool(vk_logical_device_,
			&(VkDescriptorPoolCreateInfo) {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				.maxSets = 3,
				.poolSizeCount = 3,
				.pPoolSizes = pool_sizes,
				.flags = 0,
				.pNext = NULL
		},
			NULL,
			&backend->particle_system.descriptor_pool[i]
		);
		if (res != VK_SUCCESS)
		{
			__debugbreak();
		}

	}
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT] = { backend->particle_system.particle_pipe.vk_descriptor_set_layout_,  backend->particle_system.particle_pipe.vk_descriptor_set_layout_ };
		KSAI_VK_ASSERT(vkAllocateDescriptorSets(vk_logical_device_,
			&(VkDescriptorSetAllocateInfo)
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
				.descriptorPool = backend->particle_system.descriptor_pool[i],
				.descriptorSetCount = MAX_FRAMES_IN_FLIGHT,
				.pNext = NULL,
				.pSetLayouts = layouts
		}
		, backend->particle_system.descriptor_sets
			));
	}

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorBufferInfo uniform_buffer_info = (VkDescriptorBufferInfo){
			.buffer = backend->particle_system.ubuffer[i],
			.offset = 0,
			.range = sizeof(uniforms)
		};

		VkDescriptorBufferInfo storage_buffer_info_last = (VkDescriptorBufferInfo)
		{
			.buffer = backend->particle_system.buffer[i],
			.offset = 0,
			.range = buffer_size
		};

		VkDescriptorBufferInfo storage_buffer_info_current = (VkDescriptorBufferInfo)
		{
			.buffer = backend->particle_system.buffer[i],
			.offset = 0,
			.range = buffer_size
		};

		VkWriteDescriptorSet descriptor_writes[3] = {
			(VkWriteDescriptorSet)
			{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = backend->particle_system.descriptor_sets[i],
				.dstBinding = 0,
				.dstArrayElement = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.pBufferInfo = &uniform_buffer_info
			},
			(VkWriteDescriptorSet)
			{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = backend->particle_system.descriptor_sets[i],
				.dstBinding = 1,
				.dstArrayElement = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.pBufferInfo = &storage_buffer_info_last
			},
			(VkWriteDescriptorSet)
			{
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = backend->particle_system.descriptor_sets[i],
				.dstBinding = 2,
				.dstArrayElement = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				.descriptorCount = 1,
				.pBufferInfo = &storage_buffer_info_current
			}
		};
		vkUpdateDescriptorSets(vk_logical_device_, 3, descriptor_writes, 0, NULL);
	}
}

void destroy_particles(vk_rsrs *rsrs, renderer_backend *backend)
{
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroyDescriptorPool(vk_logical_device_, backend->particle_system.descriptor_pool[i], NULL);
		vkUnmapMemory(vk_logical_device_, backend->particle_system.ubuffer_memory[i]);
		vkDestroyBuffer(vk_logical_device_, backend->particle_system.ubuffer[i], NULL);
		vkFreeMemory(vk_logical_device_, backend->particle_system.ubuffer_memory[i], NULL);

		vkUnmapMemory(vk_logical_device_, backend->particle_system.memory[i]);
		vkDestroyBuffer(vk_logical_device_, backend->particle_system.buffer[i], NULL);
		vkFreeMemory(vk_logical_device_, backend->particle_system.memory[i], NULL);
	}
	pipeline_vk_destroy3_compute(&backend->particle_system.particle_pipe);
}

void compute_particles(vk_rsrs *rsrs, renderer_backend *backend)
{
	int cf = rsrs->current_frame;
	vkCmdBindPipeline(vk_command_buffer_[cf], VK_PIPELINE_BIND_POINT_COMPUTE, backend->particle_system.particle_pipe.vk_pipeline_compute_);
	vkCmdBindDescriptorSets(
		vk_command_buffer_[cf],
		VK_PIPELINE_BIND_POINT_COMPUTE,
		backend->particle_system.particle_pipe.vk_pipeline_layout_,
		0,
		1,
		&backend->particle_system.descriptor_sets[cf],
		0,
		0
	);

	vkCmdDispatch(vk_command_buffer_[cf], MAX_NO_OF_PARTICLES / 256, 1, 1);


}

void draw_particles(vk_rsrs *rsrs, renderer_backend *backend, SDL_Event *event)
{
	uniforms uni = { 0 };

	uni = (uniforms){
		.v1[0] = (float) rand() / (float) RAND_MAX * 10
	};


	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		memcpy_s(backend->particle_system.ubffer_data[i], sizeof(uniforms), &uni, sizeof(uniforms));
	}

	int cf = rsrs->current_frame;
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindDescriptorSets(
		vk_command_buffer_[cf],
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		backend->particle_system.particle_pipe.vk_pipeline_layout_,
		0,
		1,
		&backend->particle_system.descriptor_sets[cf],
		0,
		0
	);

	vkCmdBindPipeline(
		vk_command_buffer_[cf],
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		backend->particle_system.particle_pipe.vk_pipeline_
	);

	vkCmdBindVertexBuffers(
		vk_command_buffer_[cf],
		0,
		1,
		&backend->particle_system.buffer[cf],
		offsets
	);

	vkCmdDraw(vk_command_buffer_[cf], MAX_NO_OF_PARTICLES, 1, 0, 0);
}

