#include "init.h"
#include "sync.h"
#include "run.h"
#include "resize.h"
#include "cleanup.h"
#include <ksai/ksai_memory.h>
#include <engine/objects/object.h>
#include <engine/renderer/scene.h>
#include "backend.h"
#include "pipelines.h"
#include "advanced.h"
#include "offscreen.h"

static bool first_call = true;

KSAI_API void initialize_backend(vk_rsrs *rsrs, VkInstance *instance)
{
	rsrs->current_frame = 0;
	if (first_call == true)
	{
		create_vulkan_instace(rsrs->window, instance);
	}
	first_call = false;
	create_window_surface(rsrs->window, rsrs, true, *instance);
	pick_physical_and_logical_devices(rsrs, *instance);
	create_vulkan_render_pass(rsrs);
	vulkan_sync_init(rsrs);
}

KSAI_API void initialize_renderer_backend(vk_rsrs *rsrs, renderer_backend *backend)
{
	/* CHECKER RENDERING ENGINE */
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
			sampler_layout_binding ,

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

		char *p = { "res/textures/checker.png" };
		create_vulkan_pipeline2(
			rsrs,
			&backend->checker_pipeline,
			KSAI_VK_DESCRIPTOR_POOL_SIZE,
			bindings,
			"res/shaders/renderer/checker/vshader.spv",
			"res/shaders/renderer/checker/fshader.spv",
			&binding_desp,
			1,
			attr_desp,
			6,
			1,
			&backend->checker_pipeline.vk_texture_image_,
			&backend->checker_pipeline.vk_texture_image_memory_,
			&backend->checker_pipeline.vk_texture_image_sampler_,
			&backend->checker_pipeline.vk_texture_image_view_,
			&p,
			KSAI_VK_DESCRIPTOR_POOL_SIZE,
			backend->pool_sizes
		);

		create_vulkan_pipeline3(rsrs,
			&backend->ksai_render_pipeline,
			KSAI_VK_DESCRIPTOR_POOL_SIZE,
			bindings,
			"res/shaders/renderer/ksai_render/vshader.spv",
			"res/shaders/renderer/ksai_render/fshader.spv",
			&binding_desp,
			1,
			attr_desp,
			6,
			KSAI_VK_DESCRIPTOR_POOL_SIZE,
			backend->pool_sizes
			);

		VkDeviceSize size = sizeof(kie_Vertex) * KSAI_MESH_VERTEX_MEM;
		create_buffer_util(
			size,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&backend->vbuffer,
			&backend->vbuffer_memory,
			vk_logical_device_
		);
		size = sizeof(uint32_t) * KSAI_MESH_INDEX_MEM;
		create_buffer_util(
			size,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&backend->ibuffer,
			&backend->ibuffer_memory,
			vk_logical_device_
		);


		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			size = sizeof(uniforms) * KSAI_MESH_UNIFORM_MEM;
			create_buffer_util(
				size,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				&backend->ubuffer[i],
				&backend->ubuffer_memory[i],
				vk_logical_device_
			);
			vkMapMemory(vk_logical_device_, backend->ubuffer_memory[i], 0, size, 0, &backend->udata[i]);
			memset(backend->udata[i], 0, size);
		}

		kie_Object_Arena_init();
		backend->voffsets = (VkDeviceSize *) ksai_Arena_allocate(sizeof(VkDeviceSize) * KSAI_MAX_NO_OF_OBJECTS, &global_object_arena);
		backend->ioffsets = (VkDeviceSize *) ksai_Arena_allocate(sizeof(VkDeviceSize) * KSAI_MAX_NO_OF_OBJECTS, &global_object_arena);
		backend->uoffsets = (VkDeviceSize(*)[MAX_FRAMES_IN_FLIGHT]) ksai_Arena_allocate(sizeof(VkDeviceSize) * MAX_FRAMES_IN_FLIGHT * KSAI_MAX_NO_OF_OBJECTS, &global_object_arena);

		backend->descriptor_sets = (VkDescriptorSet(*)[MAX_FRAMES_IN_FLIGHT]) ksai_Arena_allocate(sizeof(vk_dsset_pair) * KSAI_MAX_NO_OF_OBJECTS * MAX_FRAMES_IN_FLIGHT, &global_object_arena);
		backend->descriptor_pools = (VkDescriptorPool *) ksai_Arena_allocate(sizeof(VkDescriptorPool) * KSAI_MAX_NO_OF_OBJECTS, &global_object_arena);
		backend->offset_count = 0;
		backend->voffset = 0;
		backend->ioffset = 0;
		backend->uoffset[0] = 0;
		backend->uoffset[1] = 0;

		create_image_util_array(
			KSAI_TEXTURE_IMAGE_WIDTH,
			KSAI_TEXTURE_IMAGE_HEIGHT,
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			&backend->obj_images,
			&backend->obj_images_memory,
			vk_logical_device_,
			KSAI_MAX_NO_OF_TEXTURES
		);

		backend->obj_image_views = (VkImageView *) ksai_Arena_allocate(sizeof(VkImageView) * KSAI_MAX_NO_OF_OBJECTS, &global_object_arena);
		backend->obj_images_count = 0;
	}
}

KSAI_API void copy_scene_to_backend(vk_rsrs *rsrs, kie_Scene *scene, renderer_backend *backend)
{
	//backend->uoffset[0] = 0;
	//backend->uoffset[1] = 0;
	for (int i = backend->offset_count; i < scene->objects_count; i++)
	{
		vkDestroyDescriptorPool(vk_logical_device_, backend->descriptor_pools[i], NULL);
	}
	if (scene->objects_count > backend->offset_count)
	{
		for (int i = backend->offset_count; i < scene->objects_count; i++)
		{
			VkCommandPool pool;
			VkCommandPoolCreateInfo inf = (VkCommandPoolCreateInfo){
				.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				.queueFamilyIndex = 0,
				.pNext = NULL,
				.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
			};
			vkCreateCommandPool(vk_logical_device_, &inf, NULL, &pool);

			/* Vertex Buffer */
			{
				VkBuffer staging_buffer;
				VkDeviceMemory staging_buffer_memory;
				VkDeviceSize size = sizeof(kie_Vertex) * scene->objects[i].vertices_count;
				create_buffer_util(
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					&staging_buffer,
					&staging_buffer_memory,
					vk_logical_device_
				);
				void *data;
				vkMapMemory(vk_logical_device_, staging_buffer_memory, 0, size, 0, &data);
				memcpy(data, scene->objects[i].vertices, size);
				vkUnmapMemory(vk_logical_device_, staging_buffer_memory);

				VkCommandBuffer buff = begin_single_time_commands_util(pool);

				VkBufferCopy regions = (VkBufferCopy){
					.srcOffset = 0,
					.dstOffset = backend->voffset,
					.size = size
				};
				backend->voffsets[i] = backend->voffset;
				backend->voffset += size;
				vkCmdCopyBuffer(buff, staging_buffer, backend->vbuffer, 1, &regions);
				VkBufferMemoryBarrier barr = (VkBufferMemoryBarrier)
				{
					.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
					.buffer = backend->vbuffer,
					.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
					.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
					.pNext = NULL,
					.size = size,
				};
				vkCmdPipelineBarrier(
					buff,
					VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
					VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
					0,
					NULL,
					1,
					&barr,
					0,
					NULL
				);


				end_single_time_commands_util(&buff, rsrs->vk_graphics_queue_);



				vkDeviceWaitIdle(vk_logical_device_);
				vkDestroyBuffer(vk_logical_device_, staging_buffer, NULL);
				vkFreeMemory(vk_logical_device_, staging_buffer_memory, NULL);
			}

			/* Index Buffer */
			{
				VkBuffer staging_buffer;
				VkDeviceMemory staging_buffer_memory;
				VkDeviceSize size = sizeof(uint32_t) * scene->objects[i].indices_count;
				create_buffer_util(
					size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					&staging_buffer,
					&staging_buffer_memory,
					vk_logical_device_
				);
				void *data;
				vkMapMemory(vk_logical_device_, staging_buffer_memory, 0, size, 0, &data);
				memcpy(data, scene->objects[i].indices, size);
				vkUnmapMemory(vk_logical_device_, staging_buffer_memory);

				VkCommandBuffer buff = begin_single_time_commands_util(pool);

				VkBufferCopy regions = (VkBufferCopy){
					.srcOffset = 0,
					.dstOffset = backend->ioffset,
					.size = size
				};
				backend->ioffsets[i] = backend->ioffset;
				backend->ioffset += size;
				vkCmdCopyBuffer(buff, staging_buffer, backend->ibuffer, 1, &regions);

				VkBufferMemoryBarrier barr = (VkBufferMemoryBarrier)
				{
					.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
					.buffer = backend->ibuffer,
					.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
					.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
					.pNext = NULL,
					.size = size,
				};
				vkCmdPipelineBarrier(
					buff,
					VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
					VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
					0,
					NULL,
					1,
					&barr,
					0,
					NULL
				);
				end_single_time_commands_util(&buff, rsrs->vk_graphics_queue_);

				vkDeviceWaitIdle(vk_logical_device_);
				vkDestroyBuffer(vk_logical_device_, staging_buffer, NULL);
				vkFreeMemory(vk_logical_device_, staging_buffer_memory, NULL);


				vkDestroyCommandPool(vk_logical_device_, pool, NULL);
			}

			/* Descriptor Sets */
			{
				{
					VkDescriptorPoolCreateInfo pool_info = { 0 };
					pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
					pool_info.poolSizeCount = KSAI_VK_DESCRIPTOR_POOL_SIZE;
					pool_info.pPoolSizes = backend->pool_sizes;
					// we should also specify the max no of descriptro set that might be get allocated
					pool_info.maxSets = MAX_FRAMES_IN_FLIGHT;

					if (vkCreateDescriptorPool(vk_logical_device_, &pool_info, NULL, &backend->descriptor_pools[i]) != VK_SUCCESS)
					{
						__debugbreak();
						printf("Failed to create descriptor pool\n");
					}
				}

				VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT] = { backend->checker_pipeline.vk_descriptor_set_layout_, backend->checker_pipeline.vk_descriptor_set_layout_ };
				VkDescriptorSetAllocateInfo alloc_info = { 0 };
				alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				alloc_info.descriptorPool = backend->descriptor_pools[i];
				alloc_info.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
				alloc_info.pSetLayouts = layouts;

				VkResult res;
				if ((res = vkAllocateDescriptorSets(vk_logical_device_, &alloc_info, backend->descriptor_sets[i])) != VK_SUCCESS)
				{
					printf("Failed to allocate descriptor sets\n");
				}


				VkWriteDescriptorSet descriptor_writes[MAX_BUFFER_SIZE] = { 0 };
				for (size_t ii = 0; ii < MAX_FRAMES_IN_FLIGHT; ii++)
				{

					VkDeviceSize size = sizeof(uniforms);
					VkDescriptorBufferInfo buffer_info = { 0 };
					buffer_info.buffer = backend->ubuffer[ii];
					buffer_info.offset = backend->uoffset[ii];
					buffer_info.range = size; // to be changed to size
					backend->uoffsets[i][ii] = backend->uoffset[ii];
					backend->uoffset[ii] += size;

					descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptor_writes[0].dstSet = backend->descriptor_sets[i][ii];
					descriptor_writes[0].dstBinding = 0;
					descriptor_writes[0].dstArrayElement = 0;
					descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					descriptor_writes[0].descriptorCount = 1;
					descriptor_writes[0].pBufferInfo = &buffer_info;
					descriptor_writes[0].pImageInfo = NULL;
					descriptor_writes[0].pTexelBufferView = NULL;

					VkDescriptorImageInfo image_info = { 0 };
					if (i == 3)
					{
						image_info = (VkDescriptorImageInfo){
							.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
							.imageView = backend->skybox_image_view, // Yo euta matra hudaina
							.sampler = backend->skybox_sampler // Yo euta matra hudaina
						};
					}
					else
					{
						image_info = (VkDescriptorImageInfo){
							.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
							.imageView = backend->checker_pipeline.vk_texture_image_view_, // Yo euta matra hudaina
							.sampler = backend->checker_pipeline.vk_texture_image_sampler_ // Yo euta matra hudaina
						};
					}
					descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptor_writes[1].dstSet = backend->descriptor_sets[i][ii];
					descriptor_writes[1].dstBinding = 1;
					descriptor_writes[1].dstArrayElement = 0;
					descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					descriptor_writes[1].descriptorCount = 1;
					descriptor_writes[1].pBufferInfo = NULL;
					descriptor_writes[1].pImageInfo = &image_info;
					descriptor_writes[1].pTexelBufferView = NULL;

					VkDescriptorImageInfo image_info1 = { 0 };
					image_info1 = (VkDescriptorImageInfo){
						.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
						.imageView = backend->checker_pipeline.vk_texture_image_view_, // Yo euta matra hudaina
						.sampler = backend->checker_pipeline.vk_texture_image_sampler_ // Yo euta matra hudaina
					};
					descriptor_writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptor_writes[2].dstSet = backend->descriptor_sets[i][ii];
					descriptor_writes[2].dstBinding = 2;
					descriptor_writes[2].dstArrayElement = 0;
					descriptor_writes[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					descriptor_writes[2].descriptorCount = 1;
					descriptor_writes[2].pBufferInfo = NULL;
					descriptor_writes[2].pImageInfo = &image_info1;
					descriptor_writes[2].pTexelBufferView = NULL;

					VkDescriptorImageInfo image_info2 = { 0 };
					image_info2 = (VkDescriptorImageInfo){
						.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
						.imageView = backend->checker_pipeline.vk_texture_image_view_, // Yo euta matra hudaina
						.sampler = backend->checker_pipeline.vk_texture_image_sampler_ // Yo euta matra hudaina
					};
					descriptor_writes[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptor_writes[3].dstSet = backend->descriptor_sets[i][ii];
					descriptor_writes[3].dstBinding = 3;
					descriptor_writes[3].dstArrayElement = 0;
					descriptor_writes[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					descriptor_writes[3].descriptorCount = 1;
					descriptor_writes[3].pBufferInfo = NULL;
					descriptor_writes[3].pImageInfo = &image_info2;
					descriptor_writes[3].pTexelBufferView = NULL;


					VkDescriptorImageInfo image_info3 = { 0 };
					image_info3 = (VkDescriptorImageInfo){
						.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
						.imageView = backend->checker_pipeline.vk_texture_image_view_, // Yo euta matra hudaina
						.sampler = backend->checker_pipeline.vk_texture_image_sampler_ // Yo euta matra hudaina
					};
					descriptor_writes[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptor_writes[4].dstSet = backend->descriptor_sets[i][ii];
					descriptor_writes[4].dstBinding = 4;
					descriptor_writes[4].dstArrayElement = 0;
					descriptor_writes[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					descriptor_writes[4].descriptorCount = 1;
					descriptor_writes[4].pBufferInfo = NULL;
					descriptor_writes[4].pImageInfo = &image_info3;
					descriptor_writes[4].pTexelBufferView = NULL;


					vkUpdateDescriptorSets(vk_logical_device_, 5, descriptor_writes, 0, NULL);

				}
			}

		}
		backend->offset_count = scene->objects_count;
	}
}

KSAI_API void copy_scene_to_backend_reload(vk_rsrs *rsrs, kie_Scene *scene, renderer_backend *backend)
{
	for (int i = 0; i < backend->offset_count; i++)
	{
		vkDestroyDescriptorPool(vk_logical_device_, backend->descriptor_pools[i], NULL);
	}
	backend->uoffset[0] = 0;
	backend->uoffset[1] = 0;
	for (int i = 0; i < scene->objects_count; i++)
	{

		/* Descriptor Sets */
		{
			{
				VkDescriptorPoolCreateInfo pool_info = { 0 };
				pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				pool_info.poolSizeCount = KSAI_VK_DESCRIPTOR_POOL_SIZE;
				pool_info.pPoolSizes = backend->pool_sizes;
				// we should also specify the max no of descriptro set that might be get allocated
				pool_info.maxSets = MAX_FRAMES_IN_FLIGHT;

				if (vkCreateDescriptorPool(vk_logical_device_, &pool_info, NULL, &backend->descriptor_pools[i]) != VK_SUCCESS)
				{
					printf("Failed to create descriptor pool\n");
				}
			}

			VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT] = { backend->checker_pipeline.vk_descriptor_set_layout_, backend->checker_pipeline.vk_descriptor_set_layout_ };
			VkDescriptorSetAllocateInfo alloc_info = { 0 };
			alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			alloc_info.descriptorPool = backend->descriptor_pools[i];
			alloc_info.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
			alloc_info.pSetLayouts = layouts;

			VkResult res;
			if ((res = vkAllocateDescriptorSets(vk_logical_device_, &alloc_info, backend->descriptor_sets[i])) != VK_SUCCESS)
			{
				printf("Failed to allocate descriptor sets\n");
			}


			VkWriteDescriptorSet descriptor_writes[MAX_BUFFER_SIZE] = { 0 };
			for (size_t ii = 0; ii < MAX_FRAMES_IN_FLIGHT; ii++)
			{

				VkDeviceSize size = sizeof(uniforms);
				VkDescriptorBufferInfo buffer_info = { 0 };
				buffer_info.buffer = backend->ubuffer[ii];
				buffer_info.offset = backend->uoffset[ii];
				buffer_info.range = size; // to be changed to size
				backend->uoffsets[i][ii] = backend->uoffset[ii];
				backend->uoffset[ii] += size;

				descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptor_writes[0].dstSet = backend->descriptor_sets[i][ii];
				descriptor_writes[0].dstBinding = 0;
				descriptor_writes[0].dstArrayElement = 0;
				descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptor_writes[0].descriptorCount = 1;
				descriptor_writes[0].pBufferInfo = &buffer_info;
				descriptor_writes[0].pImageInfo = NULL;
				descriptor_writes[0].pTexelBufferView = NULL;

				VkDescriptorImageInfo image_info = { 0 };
				if (i == 3)
				{
					image_info = (VkDescriptorImageInfo){
						.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
						.imageView = backend->skybox_image_view, // Yo euta matra hudaina
						.sampler = backend->skybox_sampler // Yo euta matra hudaina
					};
				}
				else if (scene->objects[i].has_texture)
				{
					image_info = (VkDescriptorImageInfo){
						.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
						.imageView = backend->obj_image_views[scene->objects[i].texture_id], // Yo euta matra hudaina
						.sampler = backend->checker_pipeline.vk_texture_image_sampler_ // Yo euta matra hudaina
					};

				}
				else
				{
					image_info = (VkDescriptorImageInfo){
						.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
						.imageView = backend->checker_pipeline.vk_texture_image_view_, // Yo euta matra hudaina
						.sampler = backend->checker_pipeline.vk_texture_image_sampler_ // Yo euta matra hudaina
					};
				}
				descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptor_writes[1].dstSet = backend->descriptor_sets[i][ii];
				descriptor_writes[1].dstBinding = 1;
				descriptor_writes[1].dstArrayElement = 0;
				descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptor_writes[1].descriptorCount = 1;
				descriptor_writes[1].pBufferInfo = NULL;
				descriptor_writes[1].pImageInfo = &image_info;
				descriptor_writes[1].pTexelBufferView = NULL;

				VkDescriptorImageInfo image_info1 = { 0 };
				image_info1 = (VkDescriptorImageInfo){
					.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					.imageView = backend->checker_pipeline.vk_texture_image_view_, // Yo euta matra hudaina
					.sampler = backend->checker_pipeline.vk_texture_image_sampler_ // Yo euta matra hudaina
				};
				descriptor_writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptor_writes[2].dstSet = backend->descriptor_sets[i][ii];
				descriptor_writes[2].dstBinding = 2;
				descriptor_writes[2].dstArrayElement = 0;
				descriptor_writes[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptor_writes[2].descriptorCount = 1;
				descriptor_writes[2].pBufferInfo = NULL;
				descriptor_writes[2].pImageInfo = &image_info1;
				descriptor_writes[2].pTexelBufferView = NULL;

				VkDescriptorImageInfo image_info2 = { 0 };
				image_info2 = (VkDescriptorImageInfo){
					.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					.imageView = backend->checker_pipeline.vk_texture_image_view_, // Yo euta matra hudaina
					.sampler = backend->checker_pipeline.vk_texture_image_sampler_ // Yo euta matra hudaina
				};
				descriptor_writes[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptor_writes[3].dstSet = backend->descriptor_sets[i][ii];
				descriptor_writes[3].dstBinding = 3;
				descriptor_writes[3].dstArrayElement = 0;
				descriptor_writes[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptor_writes[3].descriptorCount = 1;
				descriptor_writes[3].pBufferInfo = NULL;
				descriptor_writes[3].pImageInfo = &image_info2;
				descriptor_writes[3].pTexelBufferView = NULL;


				VkDescriptorImageInfo image_info3 = { 0 };
				image_info3 = (VkDescriptorImageInfo){
					.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					.imageView = backend->checker_pipeline.vk_texture_image_view_, // Yo euta matra hudaina
					.sampler = backend->checker_pipeline.vk_texture_image_sampler_ // Yo euta matra hudaina
				};
				descriptor_writes[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptor_writes[4].dstSet = backend->descriptor_sets[i][ii];
				descriptor_writes[4].dstBinding = 4;
				descriptor_writes[4].dstArrayElement = 0;
				descriptor_writes[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptor_writes[4].descriptorCount = 1;
				descriptor_writes[4].pBufferInfo = NULL;
				descriptor_writes[4].pImageInfo = &image_info3;
				descriptor_writes[4].pTexelBufferView = NULL;


				vkUpdateDescriptorSets(vk_logical_device_, 5, descriptor_writes, 0, NULL);

			}
		}

	}
	backend->offset_count = scene->objects_count;
}


KSAI_API void destroy_renderer_backend(vk_rsrs *rsrs, renderer_backend *backend)
{
	vkDeviceWaitIdle(vk_logical_device_);
	vkDestroyCommandPool(vk_logical_device_, rsrs->mRenderCommandPool, NULL);
	for (int i = 0; i < backend->obj_images_count; i++)
	{
		vkDestroyImageView(vk_logical_device_, backend->obj_image_views[i], NULL);
	}
	vkDestroyImage(vk_logical_device_, backend->obj_images, NULL);
	vkFreeMemory(vk_logical_device_, backend->obj_images_memory, NULL);
	vkDestroyBuffer(vk_logical_device_, backend->vbuffer, NULL);
	vkFreeMemory(vk_logical_device_, backend->vbuffer_memory, NULL);
	vkDestroyBuffer(vk_logical_device_, backend->ibuffer, NULL);
	vkFreeMemory(vk_logical_device_, backend->ibuffer_memory, NULL);
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroyBuffer(vk_logical_device_, backend->ubuffer[i], NULL);
		vkFreeMemory(vk_logical_device_, backend->ubuffer_memory[i], NULL);
	}
	for (int i = 0; i < backend->offset_count; i++)
	{
		vkDestroyDescriptorPool(vk_logical_device_, backend->descriptor_pools[i], NULL);
	}
	pipeline_vk_destroy2(&backend->checker_pipeline);
	pipeline_vk_destroy3(&backend->ksai_render_pipeline);
	kie_Object_Arena_destroy();
}

KSAI_API int draw_backend_start(vk_rsrs *_rsrs, renderer_backend *backend)
{
	vkWaitForFences(vk_logical_device_, 1, &_rsrs->vk_inflight_fences_[_rsrs->current_frame], VK_TRUE, KSAI_U64_MAX);


	/* Acquiring an image from the swap chain */
	{
		_rsrs->result_next_image = vkAcquireNextImageKHR(vk_logical_device_, _rsrs->vk_swap_chain_, KSAI_U64_MAX, _rsrs->vk_image_available_semaphores_[_rsrs->current_frame], VK_NULL_HANDLE, &_rsrs->image_index);

		if (_rsrs->result_next_image == VK_ERROR_OUT_OF_DATE_KHR || _rsrs->result_next_image == VK_SUBOPTIMAL_KHR || frame_buffer_resized_)
		{
			frame_buffer_resized_ = 0;
			recreate_swap_chain(_rsrs);
			recreate_offscreen(_rsrs, backend);
			return -1;
		}
		else if (_rsrs->result_next_image != VK_SUCCESS && _rsrs->result_next_image != VK_SUBOPTIMAL_KHR)
		{
			printf("Failed to Aquire Swap Chain image");
			return -1;
		}
	}

	vkResetFences(
		vk_logical_device_,
		1,
		&_rsrs->vk_inflight_fences_[_rsrs->current_frame]
	);

	/* Recording the command buffer */

	vkResetCommandBuffer(vk_command_buffer_[_rsrs->current_frame], 0);
	VkCommandBufferBeginInfo begin_info = { 0 };
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = 0;
	begin_info.pInheritanceInfo = NULL;

	if (vkBeginCommandBuffer(vk_command_buffer_[_rsrs->current_frame], &begin_info) != VK_SUCCESS)
	{
		printf("FAILED to begin recording command buffer\n");
	}
}

KSAI_API void begin_cmd_buffer_off_dont_use(vk_rsrs *_rsrs)
{
	VkCommandBufferBeginInfo begin_info = { 0 };
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = 0;
	begin_info.pInheritanceInfo = NULL;
	KSAI_VK_ASSERT(vkBeginCommandBuffer(vk_command_buffer_[_rsrs->current_frame], &begin_info) != VK_SUCCESS);
}

KSAI_API void end_cmd_buffer_off_dont_use(vk_rsrs *_rsrs)
{
	KSAI_VK_ASSERT(vkEndCommandBuffer(vk_command_buffer_[_rsrs->current_frame]) != VK_SUCCESS);
}

KSAI_API int draw_backend_begin(vk_rsrs *_rsrs, vec3 color)
{

	VkRenderPassBeginInfo render_pass_info = { 0 };

	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_info.renderPass = _rsrs->vk_render_pass_;
	render_pass_info.framebuffer = _rsrs->vk_swap_chain_frame_buffers_[_rsrs->image_index];
	render_pass_info.renderArea.offset = (VkOffset2D){ 0, 0 };
	render_pass_info.renderArea.extent = _rsrs->vk_swap_chain_image_extent_2d_;


	VkClearValue clear_color[2] = {
		(VkClearValue)
		 {
		.color.float32[0] = color[0],
		.color.float32[1] = color[1],
		.color.float32[2] = color[2],
		},

		(VkClearValue)
		{
		.depthStencil = {1.0f, 0.0f}
		}
	};
	render_pass_info.clearValueCount = 2;
	render_pass_info.pClearValues = clear_color;
	vkCmdSetDepthTestEnable(vk_command_buffer_[_rsrs->current_frame], VK_FALSE);
	vkCmdBeginRenderPass(vk_command_buffer_[_rsrs->current_frame], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
	return _rsrs->current_frame;
}

KSAI_API int backend_add_texture_to_scene_object(vk_rsrs *rsrs, renderer_backend *backend, kie_Scene *scene, uint32_t obj_index, char *texture_path)
{
	VkDeviceSize image_size = (VkDeviceSize) 4 * KSAI_TEXTURE_IMAGE_WIDTH * KSAI_TEXTURE_IMAGE_HEIGHT;
	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;
	create_buffer_util(
		image_size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&staging_buffer,
		&staging_buffer_memory,
		vk_logical_device_
	);
	void *data;
	vkMapMemory(vk_logical_device_, staging_buffer_memory, 0, image_size, 0, &data);

	int tex_width, tex_height, tex_channels;
	stbi_uc *pixels = stbi_load(texture_path, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
	if (!pixels)
		printf("error, cannot load image_temporary_testing_ \n");
	memcpy(data, pixels, image_size);

	vkUnmapMemory(vk_logical_device_, staging_buffer_memory);

	transition_image_layout_util_layered(
		KSAI_MAX_NO_OF_TEXTURES,
		backend->obj_images,
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		vk_command_pool_,
		rsrs->vk_graphics_queue_
	);
	copy_buffer_to_image_util_layered_base_layer(
		1,
		backend->obj_images_count,
		staging_buffer,
		backend->obj_images,
		KSAI_TEXTURE_IMAGE_WIDTH,
		KSAI_TEXTURE_IMAGE_HEIGHT,
		vk_command_pool_,
		rsrs->vk_graphics_queue_
	);

	transition_image_layout_util_layered(
		KSAI_MAX_NO_OF_TEXTURES,
		backend->obj_images,
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		vk_command_pool_,
		rsrs->vk_graphics_queue_
	);
	vkDeviceWaitIdle(vk_logical_device_);
	vkDestroyBuffer(vk_logical_device_, staging_buffer, NULL);
	vkFreeMemory(vk_logical_device_, staging_buffer_memory, NULL);

	scene->objects[obj_index].has_texture = true;
	scene->objects[obj_index].texture_id = backend->obj_images_count;
	backend->obj_image_views[backend->obj_images_count] = create_image_view_util_base_array_layer(
		backend->obj_images,
		rsrs->vk_swap_chain_image_format_,
		VK_IMAGE_ASPECT_COLOR_BIT,
		backend->obj_images_count
	);
	backend->obj_images_count++;
	return 0;
}


KSAI_API void draw_backend_end(vk_rsrs *_rsrs)
{
	vkCmdEndRenderPass(vk_command_buffer_[_rsrs->current_frame]);

	if (vkEndCommandBuffer(vk_command_buffer_[_rsrs->current_frame]) != VK_SUCCESS)
	{
		printf("Failed to record command buffer!");
	}

}

KSAI_API void draw_backend_finish(vk_rsrs *_rsrs)
{
	VkSemaphore signal_semaphores[] = { _rsrs->vk_render_finished_semaphore_[_rsrs->current_frame] };
	submit_the_command_buffer(&_rsrs->current_frame, (int *) &_rsrs->image_index, &_rsrs->result_next_image, _rsrs);

	present_to_queue(signal_semaphores, &_rsrs->image_index, &_rsrs->result_next_image, &_rsrs->current_frame, _rsrs);

	vkQueueWaitIdle(_rsrs->vk_graphics_queue_);
}

KSAI_API void draw_backend_wait(vk_rsrs *_rsrs)
{
	vkQueueWaitIdle(_rsrs->vk_graphics_queue_);
}

KSAI_API void destroy_backend(vk_rsrs *_rsrs)
{
	vulkan_cleanup(_rsrs);
}


