#include "pipelines.h"
#include <ksai/ksai.h>
#include <ksai/ksai.h>
#include <vendor/stbi/stb_image.h>

void create_vulkan_pipeline(
	vk_rsrs *_rsrs,
	struct pipeline_vk* ppln,
	int _descriptor_set_layout_binding_count,
	VkDescriptorSetLayoutBinding *_descriptor_layout_binding,
	const char* _vshader_path,
	const char* _fshader_path,
	VkVertexInputBindingDescription *_vertex_input_binding_description,
	int _vertex_input_binding_description_count,
	VkVertexInputAttributeDescription *_vertex_input_attribute_description,
	int _vertex_input_attribute_description_count,
	int _no_of_texture_images,
	VkImage *_texture_images,
	VkDeviceMemory *_texture_image_memory,
	VkSampler *_texture_image_sampler,
	VkImageView *_texture_image_view,
	const char* *_texture_image_path,
	int _no_of_pool_sizes,
	VkDescriptorPoolSize *_pool_sizes,
	char staging_buffer_memory,
	int initial_size_factor
)
{
	/* Create Descriptor Set layout*/
	{
		VkDescriptorSetLayoutCreateInfo layout_info = { 0 };
		layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layout_info.bindingCount = _descriptor_set_layout_binding_count;
		layout_info.pBindings = _descriptor_layout_binding;

		if (vkCreateDescriptorSetLayout(vk_logical_device_, &layout_info, NULL, &ppln->vk_descriptor_set_layout_) != VK_SUCCESS)
		{
			printf("Error Settign descriptpor layouts\n");
		}
	}

	/* Create Graphics Pipeline */
	{
		char *vshader;
		int vshader_size = read_file_util( _vshader_path, &vshader);
		char *fshader;
		int fshader_size = read_file_util( _fshader_path, &fshader);

		VkShaderModule vertex_shader_module = create_shader_module_util(vshader, vshader_size);
		VkShaderModule fragment_shader_module = create_shader_module_util(fshader, fshader_size);

		VkPipelineShaderStageCreateInfo vertex_shader_info = { 0 };
		vertex_shader_info = (VkPipelineShaderStageCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = vertex_shader_module,
			.pName = "main",
			.pSpecializationInfo = NULL
			/* For compile time constants to better optimize stuffs and blah */
		};

		VkPipelineShaderStageCreateInfo fragment_shader_info = { 0 };
		fragment_shader_info = (VkPipelineShaderStageCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = fragment_shader_module,
			.pName = "main",
			.pSpecializationInfo = NULL
		};

		VkPipelineShaderStageCreateInfo shader_stages[] = { vertex_shader_info, fragment_shader_info };
		VkDynamicState dynamic_states[3] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE};

		VkPipelineDynamicStateCreateInfo dynamic_state = { 0 };
		dynamic_state = (VkPipelineDynamicStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = 3,
			.pDynamicStates = dynamic_states
		};


		VkPipelineVertexInputStateCreateInfo vertex_input_info = { 0 };
		vertex_input_info = (VkPipelineVertexInputStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = _vertex_input_binding_description_count,
			.pVertexBindingDescriptions = _vertex_input_binding_description,
			.vertexAttributeDescriptionCount = _vertex_input_attribute_description_count,
			.pVertexAttributeDescriptions = _vertex_input_attribute_description
		};

		VkPipelineInputAssemblyStateCreateInfo input_assembly = { 0 };
		input_assembly = (VkPipelineInputAssemblyStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE,
		};

		//VkViewport viewport = { 0 };
		//viewport = (VkViewport){
		//	.x = 0.0f,
		//	.y = 0.0f,
		//	.width = (float) _rsrs->vk_swap_chain_image_extent_2d_.width,
		//	.height = (float) _rsrs->vk_swap_chain_image_extent_2d_.height,
		//	.minDepth = 0.0f,
		//	.maxDepth = 1.0f
		//	/* SCISSOR RECTANGLES ARE WHERE THE PIXELS  ARE ACTUALLY STORED*/
		//};

		//VkRect2D scissor = { 0 };
		//scissor = (VkRect2D){
		//	.offset = (VkOffset2D){ 0, 0 },
		//	.extent = _rsrs->vk_swap_chain_image_extent_2d_
		//};

		VkPipelineViewportStateCreateInfo viewport_state = { 0 };
		viewport_state = (VkPipelineViewportStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.scissorCount = 1
		};

		VkPipelineRasterizationStateCreateInfo rasterizer = { 0 };
		rasterizer = (VkPipelineRasterizationStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = VK_POLYGON_MODE_FILL,
			.lineWidth = 1.0f,
			.cullMode = VK_CULL_MODE_NONE,
			.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
			.depthBiasEnable = VK_FALSE,
			.depthBiasConstantFactor = 0.0f,
				.depthBiasClamp = 0.0f,
				.depthBiasSlopeFactor = 0.0f
		};

		VkPipelineMultisampleStateCreateInfo multisampling = { 0 };
		multisampling = (VkPipelineMultisampleStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.sampleShadingEnable = VK_FALSE,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.minSampleShading = 1.0f,
			.pSampleMask = NULL,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE
		};

		/* DEPTH AND STENCIL TESTING IS ALSO RQUIRE FOR 3D BUT NOT NOW */
		VkPipelineColorBlendAttachmentState color_blend_attachment = { 0 };
		color_blend_attachment = (VkPipelineColorBlendAttachmentState){
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
			.blendEnable = VK_TRUE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
			.alphaBlendOp = VK_BLEND_OP_ADD
		};

		VkPipelineColorBlendStateCreateInfo color_blending = { 0 };
		color_blending = (VkPipelineColorBlendStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY, // Optional
			// YAA BUJHNA BAAKI XA
			.attachmentCount = 1,
			.pAttachments = &color_blend_attachment,
			.blendConstants[0] = 0.0f, // Optional
			.blendConstants[1] = 0.0f, // Optional
			.blendConstants[2] = 0.0f, // Optional
			.blendConstants[3] = 0.0f // Optional
		};

		/* ppln layouts for uniforms */
		VkPipelineLayoutCreateInfo pipeline_layout_info = { 0 };
		pipeline_layout_info = (VkPipelineLayoutCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = 1,
			.pSetLayouts = &ppln->vk_descriptor_set_layout_
		};

		VkPushConstantRange push_constant = { 0 };
		push_constant = (VkPushConstantRange){
			.offset = 0,
			.size = sizeof(struct push_constants),
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
		};

		pipeline_layout_info.pushConstantRangeCount = 1;
		pipeline_layout_info.pPushConstantRanges = &push_constant;
		/* pUSH CONSTANTS */


		/* Depth Stencil State */
		VkPipelineDepthStencilStateCreateInfo depth_stencil = (VkPipelineDepthStencilStateCreateInfo)
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable = VK_TRUE,
			.depthWriteEnable = VK_TRUE,
			.depthCompareOp = VK_COMPARE_OP_LESS,
			.depthBoundsTestEnable = VK_FALSE,
			.minDepthBounds = 0.0f,
			.maxDepthBounds = 1.0f,
			.stencilTestEnable = VK_FALSE,
			.front = {0},
			.back = {0}
		};

		if (vkCreatePipelineLayout(vk_logical_device_, &pipeline_layout_info, NULL, &ppln->vk_pipeline_layout_) != VK_SUCCESS)
		{
			printf("Failed to crreate ppln\n");
		}

		VkGraphicsPipelineCreateInfo pipeline_info = { 0 };
		pipeline_info = (VkGraphicsPipelineCreateInfo){
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.stageCount = 2,
			.pStages = shader_stages,
			.pVertexInputState = &vertex_input_info,
			.pInputAssemblyState = &input_assembly,
			.pViewportState = &viewport_state,
			.pRasterizationState = &rasterizer,
			.pMultisampleState = &multisampling,
			.pDepthStencilState = &depth_stencil,
			.pColorBlendState = &color_blending,
			.pDynamicState = &dynamic_state,
			.layout = ppln->vk_pipeline_layout_,
			.renderPass = _rsrs->vk_render_pass_,
			.subpass = 0, // Index of subpas,
			.basePipelineHandle = VK_NULL_HANDLE,
			.basePipelineIndex = -1,
		};

		if (vkCreateGraphicsPipelines(vk_logical_device_, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &ppln->vk_pipeline_) != VK_SUCCESS)
		{
			printf("Failed to create graphics ppln");
		}

		free(vshader);
		free(fshader);
		vkDestroyShaderModule(vk_logical_device_, vertex_shader_module, NULL);
		vkDestroyShaderModule(vk_logical_device_, fragment_shader_module, NULL);
	}

	/* Texture Image */
	for (int i = 0; i < _no_of_texture_images; i++)
	{
		/* Create Texture Image */
		{
			int tex_width, tex_height, tex_channels;
			stbi_set_flip_vertically_on_load(true);
			stbi_uc* pixels = stbi_load(_texture_image_path[i], &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
			VkDeviceSize image_size = tex_width * tex_height * 4;
			if (!pixels)
				printf("error, cannot load image_temporary_testing_ \n");
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

			void* data;
			vkMapMemory(vk_logical_device_, staging_buffer_memory, 0, image_size, 0, &data);
			memcpy(data, pixels, (size_t)image_size);
			vkUnmapMemory(vk_logical_device_, staging_buffer_memory);
			stbi_image_free(pixels);
			create_image_util(tex_width, tex_height,
				VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&_texture_images[i],
				&_texture_image_memory[i],
				vk_logical_device_
			);
			transition_image_layout_util(
				_texture_images[i],
				VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				vk_command_pool_,
				_rsrs->vk_graphics_queue_
			);
			copy_buffer_to_image_util(
				staging_buffer,
				_texture_images[i],
				tex_width,
				tex_height,
				vk_command_pool_,
				_rsrs->vk_graphics_queue_
			);
			transition_image_layout_util(
				_texture_images[i],
				VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				vk_command_pool_,
				_rsrs->vk_graphics_queue_
			);
			vkDeviceWaitIdle(vk_logical_device_);
			vkDestroyBuffer(vk_logical_device_, staging_buffer, NULL);
			vkFreeMemory(vk_logical_device_, staging_buffer_memory, NULL);

			VkImageViewCreateInfo create_info = { 0 };
			create_info = (VkImageViewCreateInfo)
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = _texture_images[i],
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = VK_FORMAT_R8G8B8A8_SRGB,
				.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.subresourceRange.baseMipLevel = 0,
				.subresourceRange.levelCount = 1,
				.subresourceRange.baseArrayLayer = 0,
				.subresourceRange.layerCount = 1,
				.components = {VK_COMPONENT_SWIZZLE_IDENTITY}
			};

			if (vkCreateImageView(vk_logical_device_, &create_info, NULL, &_texture_image_view[i]) != VK_SUCCESS)
			{
				printf("Failed to create image_temporary_testing_ views \n");
			}

			/* Create texture Sampler */
			{
				create_texture_sampler(
					&_texture_image_sampler[i],
					vk_logical_device_,
					vk_physical_device_
				);
			}
		}
	}


	if (!(staging_buffer_memory & STAGING_BUFFER_VERTEX_ON))
	{
		/* Create Vertex Buffers */
		{
			VkBuffer staging_buffer;
			VkDeviceMemory staging_buffer_memory;
			VkDeviceSize size = ppln->vertices_count * sizeof(ppln->vertices[0]);

			/* Create Staging buffer If not rapidly changing */
			{
				{
					VkBufferCreateInfo create_info = { 0 };
					create_info = (VkBufferCreateInfo)
					{
						.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
						.size = size,
						.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						.sharingMode = VK_SHARING_MODE_EXCLUSIVE
					};

					if (vkCreateBuffer(vk_logical_device_, &create_info, NULL, &staging_buffer) != VK_SUCCESS)
					{
						printf("Failed to create vertex Buffers\n");
					}
				}

				/* Memory Requirement and Allocating Memory */
				{
					VkMemoryRequirements memory_requirements = { 0 };
					vkGetBufferMemoryRequirements(vk_logical_device_, staging_buffer, &memory_requirements);

					VkMemoryAllocateInfo alloc_info = { 0 };
					alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
					alloc_info.allocationSize = memory_requirements.size;
					alloc_info.memoryTypeIndex = find_memory_type_util(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
					/* yo chae mazzale nai bujhna baaki xa */

					if (vkAllocateMemory(vk_logical_device_, &alloc_info, NULL, &staging_buffer_memory) != VK_SUCCESS)
					{
						printf("Failed to Allocate Vertex Buffer Memory\n");
					}
					// Allocate matrai haina, bind ni garnu paro buffer sanga, buffer ko memory chae yo ho hai vnna paron ta lul
					vkBindBufferMemory(vk_logical_device_, staging_buffer, staging_buffer_memory, 0);
					void* data;
					vkMapMemory(vk_logical_device_, staging_buffer_memory, 0, size, 0, &data);
					memcpy(data, ppln->vertices, (size_t)size);
					vkUnmapMemory(vk_logical_device_, staging_buffer_memory);
				}
			}
			/* Create Vertex Actual Buffers*/
			{
				{
					VkBufferCreateInfo create_info = { 0 };
					create_info = (VkBufferCreateInfo)
					{
						.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
						.size = size,
						.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
						.sharingMode = VK_SHARING_MODE_EXCLUSIVE
					};

					if (vkCreateBuffer(vk_logical_device_, &create_info, NULL, &ppln->vk_vertex_buffer_) != VK_SUCCESS)
					{
						printf("Failed to create vertex Buffers\n");
					}
				}

				/* Memory Requirement and Allocating Memory */
				{
					VkMemoryRequirements memory_requirements = { 0 };
					vkGetBufferMemoryRequirements(vk_logical_device_, ppln->vk_vertex_buffer_, &memory_requirements);

					VkMemoryAllocateInfo alloc_info = { 0 };
					alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
					alloc_info.allocationSize = memory_requirements.size;
					alloc_info.memoryTypeIndex = find_memory_type_util(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
					/* yo chae mazzale nai bujhna baaki xa */

					if (vkAllocateMemory(vk_logical_device_, &alloc_info, NULL, &ppln->vk_vertex_buffer_memory_) != VK_SUCCESS)
					{
						printf("Failed to Allocate Vertex Buffer Memory\n");
					}
					// Allocate matrai haina, bind ni garnu paro buffer sanga, buffer ko memory chae yo ho hai vnna paron ta lul
					vkBindBufferMemory(vk_logical_device_, ppln->vk_vertex_buffer_, ppln->vk_vertex_buffer_memory_, 0);
				}

			}
			/* Copy the data from staging buffer to vertex buffer */
			{
				VkCommandPool new_command_pool;
				VkCommandPoolCreateInfo command_pool_create_info = { 0 };
				command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
				command_pool_create_info.queueFamilyIndex = 0; // matlab graphics Queue

				if (vkCreateCommandPool(vk_logical_device_, &command_pool_create_info, NULL, &new_command_pool) != VK_SUCCESS)
				{
					printf("Command Pool creation failed \n");
				}

				VkCommandBufferAllocateInfo alloc_info = { 0 };
				alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				alloc_info.commandBufferCount = 1;
				alloc_info.commandPool = new_command_pool;

				VkCommandBuffer command_buffer;
				vkAllocateCommandBuffers(vk_logical_device_, &alloc_info, &command_buffer);

				VkCommandBufferBeginInfo begin_info = { 0 };
				begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

				vkBeginCommandBuffer(command_buffer, &begin_info);

				VkBufferCopy copy_region = { 0 };
				copy_region.srcOffset = 0;
				copy_region.dstOffset = 0;
				copy_region.size = size;
				vkCmdCopyBuffer(command_buffer, staging_buffer, ppln->vk_vertex_buffer_, 1, &copy_region);
				vkEndCommandBuffer(command_buffer);

				VkSubmitInfo submit_info = { 0 };
				submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submit_info.commandBufferCount = 1;
				submit_info.pCommandBuffers = &command_buffer;
				vkQueueSubmit(_rsrs->vk_graphics_queue_, 1, &submit_info, VK_NULL_HANDLE);
				vkQueueWaitIdle(_rsrs->vk_graphics_queue_);

				vkFreeCommandBuffers(vk_logical_device_, new_command_pool, 1, &command_buffer);
				vkDestroyCommandPool(vk_logical_device_, new_command_pool, NULL);
			}

			/* Destroy buffers */
			{
				vkDestroyBuffer(vk_logical_device_, staging_buffer, NULL);
				vkFreeMemory(vk_logical_device_, staging_buffer_memory, NULL);
			}
		}
	}
	else {
		/* Create Vertex Buffers */
		{
			VkDeviceSize size = ppln->vertices_count * sizeof(ppln->vertices[0]) * initial_size_factor;

			/* Create Staging buffer If not rapidly changing */
			{
				{
					VkBufferCreateInfo create_info = { 0 };
					create_info = (VkBufferCreateInfo)
					{
						.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
						.size = size,
						.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
						.sharingMode = VK_SHARING_MODE_EXCLUSIVE
					};

					if (vkCreateBuffer(vk_logical_device_, &create_info, NULL, &ppln->vk_vertex_buffer_) != VK_SUCCESS)
					{
						printf("Failed to create vertex Buffers\n");
					}
				}

				/* Memory Requirement and Allocating Memory */
				{
					VkMemoryRequirements memory_requirements = { 0 };
					vkGetBufferMemoryRequirements(vk_logical_device_, ppln->vk_vertex_buffer_, &memory_requirements);

					VkMemoryAllocateInfo alloc_info = { 0 };
					alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
					alloc_info.allocationSize = memory_requirements.size;
					alloc_info.memoryTypeIndex = find_memory_type_util(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
					/* yo chae mazzale nai bujhna baaki xa */

					if (vkAllocateMemory(vk_logical_device_, &alloc_info, NULL, &ppln->vk_vertex_buffer_memory_) != VK_SUCCESS)
					{
						printf("Failed to Allocate Vertex Buffer Memory\n");
					}
					vkBindBufferMemory(vk_logical_device_, ppln->vk_vertex_buffer_, ppln->vk_vertex_buffer_memory_, 0);
					//void* data;
					//vkMapMemory(vk_logical_device_, ppln->vk_vertex_buffer_memory_, 0, size, 0, &data);
					//memcpy(data, ppln->vertices, (size_t)size);
					//vkUnmapMemory(vk_logical_device_, ppln->vk_vertex_buffer_memory_);
				}
			}
		}

	}


	/* Create Index Buffers */
	if (!(staging_buffer_memory & STAGING_BUFFER_INDEX_ON))
	{
		VkBuffer staging_buffer;
		VkDeviceMemory staging_buffer_memory;
		VkDeviceSize size = ppln->indices_count * sizeof(ppln->indcs[0]);

		/* Create Staging buffer If not rapidly changing */
		{
			{
				VkBufferCreateInfo create_info = { 0 };
				create_info = (VkBufferCreateInfo)
				{
					.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
					.size = size,
					.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					.sharingMode = VK_SHARING_MODE_EXCLUSIVE
				};

				if (vkCreateBuffer(vk_logical_device_, &create_info, NULL, &staging_buffer) != VK_SUCCESS)
				{
					printf("Failed to create vertex Buffers\n");
				}
			}

			/* Memory Requirement and Allocating Memory */
			{
				VkMemoryRequirements memory_requirements = { 0 };
				vkGetBufferMemoryRequirements(vk_logical_device_, staging_buffer, &memory_requirements);

				VkMemoryAllocateInfo alloc_info = { 0 };
				alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				alloc_info.allocationSize = memory_requirements.size;
				alloc_info.memoryTypeIndex = find_memory_type_util(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
				/* yo chae mazzale nai bujhna baaki xa */

				if (vkAllocateMemory(vk_logical_device_, &alloc_info, NULL, &staging_buffer_memory) != VK_SUCCESS)
				{
					printf("Failed to Allocate Vertex Buffer Memory\n");
				}
				// Allocate matrai haina, bind ni garnu paro buffer sanga, buffer ko memory chae yo ho hai vnna paron ta lul
				vkBindBufferMemory(vk_logical_device_, staging_buffer, staging_buffer_memory, 0);
				void* data;
				vkMapMemory(vk_logical_device_, staging_buffer_memory, 0, size, 0, &data);
				memcpy(data, ppln->indcs, (size_t)size);
				vkUnmapMemory(vk_logical_device_, staging_buffer_memory);
			}
		}
		/* Create Index Actual Buffers*/
		{
			{
				VkBufferCreateInfo create_info = { 0 };
				create_info = (VkBufferCreateInfo)
				{
					.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
					.size = size,
					.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
					.sharingMode = VK_SHARING_MODE_EXCLUSIVE
				};

				if (vkCreateBuffer(vk_logical_device_, &create_info, NULL, &ppln->vk_index_buffer_) != VK_SUCCESS)
				{
					printf("Failed to create vertex Buffers\n");
				}
			}

			/* Memory Requirement and Allocating Memory */
			{
				VkMemoryRequirements memory_requirements = { 0 };
				vkGetBufferMemoryRequirements(vk_logical_device_, ppln->vk_index_buffer_, &memory_requirements);

				VkMemoryAllocateInfo alloc_info = { 0 };
				alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				alloc_info.allocationSize = memory_requirements.size;
				alloc_info.memoryTypeIndex = find_memory_type_util(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
				/* yo chae mazzale nai bujhna baaki xa */

				if (vkAllocateMemory(vk_logical_device_, &alloc_info, NULL, &ppln->vk_index_buffer_memory_) != VK_SUCCESS)
				{
					printf("Failed to Allocate Vertex Buffer Memory\n");
				}
				// Allocate matrai haina, bind ni garnu paro buffer sanga, buffer ko memory chae yo ho hai vnna paron ta lul
				vkBindBufferMemory(vk_logical_device_, ppln->vk_index_buffer_, ppln->vk_index_buffer_memory_, 0);
			}

		}
		/* Copy the data from staging buffer to index buffer */
		{
			VkCommandPool new_command_pool;
			VkCommandPoolCreateInfo command_pool_create_info = { 0 };
			command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
			command_pool_create_info.queueFamilyIndex = 0; // matlab graphics Queue

			if (vkCreateCommandPool(vk_logical_device_, &command_pool_create_info, NULL, &new_command_pool) != VK_SUCCESS)
			{
				printf("Command Pool creation failed \n");
			}

			VkCommandBufferAllocateInfo alloc_info = { 0 };
			alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			alloc_info.commandBufferCount = 1;
			alloc_info.commandPool = new_command_pool;

			VkCommandBuffer command_buffer;
			vkAllocateCommandBuffers(vk_logical_device_, &alloc_info, &command_buffer);

			VkCommandBufferBeginInfo begin_info = { 0 };
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(command_buffer, &begin_info);

			VkBufferCopy copy_region = { 0 };
			copy_region.srcOffset = 0;
			copy_region.dstOffset = 0;
			copy_region.size = size;
			vkCmdCopyBuffer(command_buffer, staging_buffer, ppln->vk_index_buffer_, 1, &copy_region);
			vkEndCommandBuffer(command_buffer);

			VkSubmitInfo submit_info = { 0 };
			submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info.commandBufferCount = 1;
			submit_info.pCommandBuffers = &command_buffer;
			vkQueueSubmit(_rsrs->vk_graphics_queue_, 1, &submit_info, VK_NULL_HANDLE);
			vkQueueWaitIdle(_rsrs->vk_graphics_queue_);

			vkFreeCommandBuffers(vk_logical_device_, new_command_pool, 1, &command_buffer);
			vkDestroyCommandPool(vk_logical_device_, new_command_pool, NULL);
		}

		/* Destroy buffers */
		{
			vkDestroyBuffer(vk_logical_device_, staging_buffer, NULL);
			vkFreeMemory(vk_logical_device_, staging_buffer_memory, NULL);
		}
	}
	 else {
		/* Create INdex Buffers */
		{
			VkDeviceSize size = ppln->indices_count * sizeof(ppln->indcs[0]) * initial_size_factor;

			/* Create Staging buffer If not rapidly changing */
			{
				{
					VkBufferCreateInfo create_info = { 0 };
					create_info = (VkBufferCreateInfo)
					{
						.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
						.size = size,
						.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
						.sharingMode = VK_SHARING_MODE_EXCLUSIVE
					};

					if (vkCreateBuffer(vk_logical_device_, &create_info, NULL, &ppln->vk_index_buffer_) != VK_SUCCESS)
					{
						printf("Failed to create vertex Buffers\n");
					}
				}

				/* Memory Requirement and Allocating Memory */
				{
					VkMemoryRequirements memory_requirements = { 0 };
					vkGetBufferMemoryRequirements(vk_logical_device_, ppln->vk_index_buffer_, &memory_requirements);

					VkMemoryAllocateInfo alloc_info = { 0 };
					alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
					alloc_info.allocationSize = memory_requirements.size;
					alloc_info.memoryTypeIndex = find_memory_type_util(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
					/* yo chae mazzale nai bujhna baaki xa */

					if (vkAllocateMemory(vk_logical_device_, &alloc_info, NULL, &ppln->vk_index_buffer_memory_) != VK_SUCCESS)
					{
						printf("Failed to Allocate Vertex Buffer Memory\n");
					}
					vkBindBufferMemory(vk_logical_device_, ppln->vk_index_buffer_, ppln->vk_index_buffer_memory_, 0);
				}
			}
		}


	}


	/*Create Uniform buffers*/
	{
		VkDeviceSize buffer_size = sizeof(ppln->ubo);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			/* Create the Actual Uniform Buffer */
			{
				VkBufferCreateInfo create_info = { 0 };
				create_info = (VkBufferCreateInfo)
				{
					.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
					.size = buffer_size,
					.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					.sharingMode = VK_SHARING_MODE_EXCLUSIVE
				};

				if (vkCreateBuffer(vk_logical_device_, &create_info, NULL, &ppln->vk_uniform_buffer_[i]) != VK_SUCCESS)
				{
					printf("Failed to create vertex Buffers\n");
				}

				/*Allocate the memory*/
				{
					VkMemoryRequirements memory_requirements = { 0 };
					vkGetBufferMemoryRequirements(vk_logical_device_, ppln->vk_uniform_buffer_[i], &memory_requirements);

					VkMemoryAllocateInfo alloc_info = { 0 };
					alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
					alloc_info.allocationSize = memory_requirements.size;
					alloc_info.memoryTypeIndex = find_memory_type_util(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
					/* yo chae mazzale nai bujhna baaki xa */

					if (vkAllocateMemory(vk_logical_device_, &alloc_info, NULL, &ppln->vk_uniform_buffer_memory_[i]) != VK_SUCCESS)
					{
						printf("Failed to Allocate Vertex Buffer Memory\n");
					}
					// Allocate matrai haina, bind ni garnu paro buffer sanga, buffer ko memory chae yo ho hai vnna paron ta lul
					vkBindBufferMemory(vk_logical_device_, ppln->vk_uniform_buffer_[i], ppln->vk_uniform_buffer_memory_[i], 0);
				}
			}
			vkMapMemory(vk_logical_device_, ppln->vk_uniform_buffer_memory_[i], 0, buffer_size, 0, &ppln->vk_uniform_buffer_mapped_region_data_[i]);
			/* Persistent mapping technique the GPU memory remains mapped with the cpu memory upto the whole time of the aplication*/
		}
	}

	/* Create descriptors pool */
	{
		VkDescriptorPoolCreateInfo pool_info = { 0 };
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.poolSizeCount = _no_of_pool_sizes;
		pool_info.pPoolSizes = _pool_sizes;
		// we should also specify the max no of descriptro set that might be get allocated
		pool_info.maxSets = MAX_FRAMES_IN_FLIGHT;

		if (vkCreateDescriptorPool(vk_logical_device_, &pool_info, NULL, &ppln->vk_descriptor_pool_) != VK_SUCCESS)
		{
			printf("Failed to create descriptor pool\n");
		}
	}

	/* Create descriptor Set */
	{
		VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT] = { ppln->vk_descriptor_set_layout_ , ppln->vk_descriptor_set_layout_ };
		VkDescriptorSetAllocateInfo alloc_info = { 0 };
		alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		alloc_info.descriptorPool = ppln->vk_descriptor_pool_;
		alloc_info.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
		alloc_info.pSetLayouts = layouts;

		VkResult res;
		if ((res = vkAllocateDescriptorSets(vk_logical_device_, &alloc_info, ppln->vk_descriptor_sets_)) != VK_SUCCESS)
		{
			printf("Failed to allocate descriptor sets\n");
		}

		static VkWriteDescriptorSet descriptor_writes[MAX_BUFFER_SIZE] = { 0 };
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{

			VkDescriptorBufferInfo buffer_info = { 0 };
			buffer_info.buffer = ppln->vk_uniform_buffer_[i];
			buffer_info.offset = 0;
			buffer_info.range = VK_WHOLE_SIZE;

			descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_writes[0].dstSet = ppln->vk_descriptor_sets_[i];
			descriptor_writes[0].dstBinding = 0;
			descriptor_writes[0].dstArrayElement = 0;
			descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptor_writes[0].descriptorCount = 1;
			descriptor_writes[0].pBufferInfo = &buffer_info;
			descriptor_writes[0].pImageInfo = NULL;
			descriptor_writes[0].pTexelBufferView = NULL;

			for (int j = 1; j <= _no_of_texture_images; j++)
			{
				VkDescriptorImageInfo image_info = { 0 };
				image_info = (VkDescriptorImageInfo){
					.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					.imageView = _texture_image_view[j - 1], // Yo euta matra hudaina
					.sampler = _texture_image_sampler[j - 1] // Yo euta matra hudaina
				};
				descriptor_writes[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptor_writes[j].dstSet = ppln->vk_descriptor_sets_[i];
				descriptor_writes[j].dstBinding = j;
				descriptor_writes[j].dstArrayElement = 0;
				descriptor_writes[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptor_writes[j].descriptorCount = 1;
				descriptor_writes[j].pBufferInfo = NULL;
				descriptor_writes[j].pImageInfo = &image_info;
				descriptor_writes[j].pTexelBufferView = NULL;
			}
			vkUpdateDescriptorSets(vk_logical_device_, 1 + _no_of_texture_images, descriptor_writes, 0, NULL);

		}
	}

}

void create_vulkan_pipeline2(
	vk_rsrs *_rsrs,
	struct pipeline_vk* ppln,
	int _descriptor_set_layout_binding_count,
	VkDescriptorSetLayoutBinding *_descriptor_layout_binding,
	const char* _vshader_path,
	const char* _fshader_path,
	VkVertexInputBindingDescription *_vertex_input_binding_description,
	int _vertex_input_binding_description_count,
	VkVertexInputAttributeDescription *_vertex_input_attribute_description,
	int _vertex_input_attribute_description_count,
	int _no_of_texture_images,
	VkImage *_texture_images,
	VkDeviceMemory *_texture_image_memory,
	VkSampler *_texture_image_sampler,
	VkImageView *_texture_image_view,
	const char* *_texture_image_path,
	int _no_of_pool_sizes,
	VkDescriptorPoolSize *_pool_sizes
)
{
	/* Create Descriptor Set layout*/
	{
		VkDescriptorSetLayoutCreateInfo layout_info = { 0 };
		layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layout_info.bindingCount = _descriptor_set_layout_binding_count;
		layout_info.pBindings = _descriptor_layout_binding;

		if (vkCreateDescriptorSetLayout(vk_logical_device_, &layout_info, NULL, &ppln->vk_descriptor_set_layout_) != VK_SUCCESS)
		{
			printf("Error Settign descriptpor layouts\n");
		}
	}

	/* Create Graphics Pipeline */
	{
		char *vshader = NULL;
		int vshader_size = read_file_util(_vshader_path, &vshader);
		char *fshader = NULL;
		int fshader_size = read_file_util(_fshader_path, &fshader);

		VkShaderModule vertex_shader_module = create_shader_module_util(vshader, vshader_size);
		VkShaderModule fragment_shader_module = create_shader_module_util(fshader, fshader_size);

		VkPipelineShaderStageCreateInfo vertex_shader_info = { 0 };
		vertex_shader_info = (VkPipelineShaderStageCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = vertex_shader_module,
			.pName = "main",
			.pSpecializationInfo = NULL
			/* For compile time constants to better optimize stuffs and blah */
		};

		VkPipelineShaderStageCreateInfo fragment_shader_info = { 0 };
		fragment_shader_info = (VkPipelineShaderStageCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = fragment_shader_module,
			.pName = "main",
			.pSpecializationInfo = NULL
		};

		VkPipelineShaderStageCreateInfo shader_stages[] = { vertex_shader_info, fragment_shader_info };
		VkDynamicState dynamic_states[3] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE };

		VkPipelineDynamicStateCreateInfo dynamic_state = { 0 };
		dynamic_state = (VkPipelineDynamicStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = 3,
			.pDynamicStates = dynamic_states
		};


		VkPipelineVertexInputStateCreateInfo vertex_input_info = { 0 };
		vertex_input_info = (VkPipelineVertexInputStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = _vertex_input_binding_description_count,
			.pVertexBindingDescriptions = _vertex_input_binding_description,
			.vertexAttributeDescriptionCount = _vertex_input_attribute_description_count,
			.pVertexAttributeDescriptions = _vertex_input_attribute_description
		};

		VkPipelineInputAssemblyStateCreateInfo input_assembly = { 0 };
		input_assembly = (VkPipelineInputAssemblyStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE,
		};

		//VkViewport viewport = { 0 };
		//viewport = (VkViewport){
		//	.x = 0.0f,
		//	.y = 0.0f,
		//	.width = (float) _rsrs->vk_swap_chain_image_extent_2d_.width,
		//	.height = (float) _rsrs->vk_swap_chain_image_extent_2d_.height,
		//	.minDepth = 0.0f,
		//	.maxDepth = 1.0f
		//	/* SCISSOR RECTANGLES ARE WHERE THE PIXELS  ARE ACTUALLY STORED*/
		//};

		//VkRect2D scissor = { 0 };
		//scissor = (VkRect2D){
		//	.offset = (VkOffset2D){ 0, 0 },
		//	.extent = _rsrs->vk_swap_chain_image_extent_2d_
		//};

		VkPipelineViewportStateCreateInfo viewport_state = { 0 };
		viewport_state = (VkPipelineViewportStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.scissorCount = 1
		};

		VkPipelineRasterizationStateCreateInfo rasterizer = { 0 };
		rasterizer = (VkPipelineRasterizationStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = VK_POLYGON_MODE_FILL,
			.lineWidth = 1.0f,
			.cullMode = VK_CULL_MODE_NONE,
			.frontFace = VK_FRONT_FACE_CLOCKWISE,
			.depthBiasEnable = VK_FALSE,
			.depthBiasConstantFactor = 0.0f,
				.depthBiasClamp = 0.0f,
				.depthBiasSlopeFactor = 0.0f
		};

		VkPipelineMultisampleStateCreateInfo multisampling = { 0 };
		multisampling = (VkPipelineMultisampleStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.sampleShadingEnable = VK_FALSE,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.minSampleShading = 1.0f,
			.pSampleMask = NULL,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE
		};

		/* DEPTH AND STENCIL TESTING IS ALSO RQUIRE FOR 3D BUT NOT NOW */
		VkPipelineColorBlendAttachmentState color_blend_attachment = { 0 };
		color_blend_attachment = (VkPipelineColorBlendAttachmentState){
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
			.blendEnable = VK_TRUE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
			.alphaBlendOp = VK_BLEND_OP_ADD
		};

		VkPipelineColorBlendStateCreateInfo color_blending = { 0 };
		color_blending = (VkPipelineColorBlendStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY, // Optional
			// YAA BUJHNA BAAKI XA
			.attachmentCount = 1,
			.pAttachments = &color_blend_attachment,
			.blendConstants[0] = 0.0f, // Optional
			.blendConstants[1] = 0.0f, // Optional
			.blendConstants[2] = 0.0f, // Optional
			.blendConstants[3] = 0.0f // Optional
		};

		/* ppln layouts for uniforms */
		VkPipelineLayoutCreateInfo pipeline_layout_info = { 0 };
		pipeline_layout_info = (VkPipelineLayoutCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = 1,
			.pSetLayouts = &ppln->vk_descriptor_set_layout_
		};

		VkPushConstantRange push_constant = { 0 };
		push_constant = (VkPushConstantRange){
			.offset = 0,
			.size = sizeof(struct push_constants),
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
		};

		pipeline_layout_info.pushConstantRangeCount = 1;
		pipeline_layout_info.pPushConstantRanges = &push_constant;
		/* pUSH CONSTANTS */

		/* Depth Stencil State */
		VkPipelineDepthStencilStateCreateInfo depth_stencil = (VkPipelineDepthStencilStateCreateInfo)
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable = VK_TRUE,
			.depthWriteEnable = VK_TRUE,
			.depthCompareOp = VK_COMPARE_OP_LESS,
			.depthBoundsTestEnable = VK_FALSE,
			.minDepthBounds = 0.0f,
			.maxDepthBounds = 1.0f,
			.stencilTestEnable = VK_FALSE,
			.front = {0},
			.back = {0}
		};

		if (vkCreatePipelineLayout(vk_logical_device_, &pipeline_layout_info, NULL, &ppln->vk_pipeline_layout_) != VK_SUCCESS)
		{
			printf("Failed to crreate ppln\n");
		}

		VkGraphicsPipelineCreateInfo pipeline_info = { 0 };
		pipeline_info = (VkGraphicsPipelineCreateInfo){
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.stageCount = 2,
			.pStages = shader_stages,
			.pVertexInputState = &vertex_input_info,
			.pInputAssemblyState = &input_assembly,
			.pViewportState = &viewport_state,
			.pRasterizationState = &rasterizer,
			.pMultisampleState = &multisampling,
			.pDepthStencilState = &depth_stencil,
			.pColorBlendState = &color_blending,
			.pDynamicState = &dynamic_state,
			.layout = ppln->vk_pipeline_layout_,
			.renderPass = _rsrs->vk_render_pass_,
			.subpass = 0, // Index of subpas,
			.basePipelineHandle = VK_NULL_HANDLE,
			.basePipelineIndex = -1,
		};

		if (vkCreateGraphicsPipelines(vk_logical_device_, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &ppln->vk_pipeline_) != VK_SUCCESS)
		{
			printf("Failed to create graphics ppln");
		}

		free(vshader);
		free(fshader);
		vkDestroyShaderModule(vk_logical_device_, vertex_shader_module, NULL);
		vkDestroyShaderModule(vk_logical_device_, fragment_shader_module, NULL);
	}

	/* Texture Image */
	for (int i = 0; i < _no_of_texture_images; i++)
	{
		/* Create Texture Image */
		{
			int tex_width, tex_height, tex_channels;
			stbi_uc* pixels;
			stbi_set_flip_vertically_on_load(true);
			if(strcmp(_texture_image_path[i], "") == 0)
				pixels = stbi_load("res/textures/checker.png", &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
			else
				pixels = stbi_load(_texture_image_path[i], &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

			VkDeviceSize image_size = tex_width * tex_height * 4;
			if (!pixels)
				printf("error, cannot load image_temporary_testing_ \n");
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

			void* data;
			vkMapMemory(vk_logical_device_, staging_buffer_memory, 0, image_size, 0, &data);
			memcpy(data, pixels, (size_t)image_size);
			vkUnmapMemory(vk_logical_device_, staging_buffer_memory);
			stbi_image_free(pixels);
			create_image_util(tex_width, tex_height,
				VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&_texture_images[i],
				&_texture_image_memory[i],
				vk_logical_device_
			);
			transition_image_layout_util(
				_texture_images[i],
				VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				vk_command_pool_,
				_rsrs->vk_graphics_queue_
			);
			copy_buffer_to_image_util(
				staging_buffer,
				_texture_images[i],
				tex_width,
				tex_height,
				vk_command_pool_,
				_rsrs->vk_graphics_queue_
			);
			transition_image_layout_util(
				_texture_images[i],
				VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				vk_command_pool_,
				_rsrs->vk_graphics_queue_
			);
			vkDeviceWaitIdle(vk_logical_device_);
			vkDestroyBuffer(vk_logical_device_, staging_buffer, NULL);
			vkFreeMemory(vk_logical_device_, staging_buffer_memory, NULL);

			VkImageViewCreateInfo create_info = { 0 };
			create_info = (VkImageViewCreateInfo)
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = _texture_images[i],
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = VK_FORMAT_R8G8B8A8_SRGB,
				.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.subresourceRange.baseMipLevel = 0,
				.subresourceRange.levelCount = 1,
				.subresourceRange.baseArrayLayer = 0,
				.subresourceRange.layerCount = 1,
				.components = {VK_COMPONENT_SWIZZLE_IDENTITY}
			};

			if (vkCreateImageView(vk_logical_device_, &create_info, NULL, &_texture_image_view[i]) != VK_SUCCESS)
			{
				printf("Failed to create image_temporary_testing_ views \n");
			}

			/* Create texture Sampler */
			{
				create_texture_sampler(
					&_texture_image_sampler[i],
					vk_logical_device_,
					vk_physical_device_
				);
			}
		}
	}

	/*Create Uniform buffers*/
	{
		VkDeviceSize buffer_size = sizeof(ppln->ubo);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			/* Create the Actual Uniform Buffer */
			{
				VkBufferCreateInfo create_info = { 0 };
				create_info = (VkBufferCreateInfo)
				{
					.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
					.size = buffer_size,
					.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					.sharingMode = VK_SHARING_MODE_EXCLUSIVE
				};

				if (vkCreateBuffer(vk_logical_device_, &create_info, NULL, &ppln->vk_uniform_buffer_[i]) != VK_SUCCESS)
				{
					printf("Failed to create vertex Buffers\n");
				}

				/*Allocate the memory*/
				{
					VkMemoryRequirements memory_requirements = { 0 };
					vkGetBufferMemoryRequirements(vk_logical_device_, ppln->vk_uniform_buffer_[i], &memory_requirements);

					VkMemoryAllocateInfo alloc_info = { 0 };
					alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
					alloc_info.allocationSize = memory_requirements.size;
					alloc_info.memoryTypeIndex = find_memory_type_util(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
					/* yo chae mazzale nai bujhna baaki xa */

					if (vkAllocateMemory(vk_logical_device_, &alloc_info, NULL, &ppln->vk_uniform_buffer_memory_[i]) != VK_SUCCESS)
					{
						printf("Failed to Allocate Vertex Buffer Memory\n");
					}
					// Allocate matrai haina, bind ni garnu paro buffer sanga, buffer ko memory chae yo ho hai vnna paron ta lul
					vkBindBufferMemory(vk_logical_device_, ppln->vk_uniform_buffer_[i], ppln->vk_uniform_buffer_memory_[i], 0);
				}
			}
			vkMapMemory(vk_logical_device_, ppln->vk_uniform_buffer_memory_[i], 0, buffer_size, 0, &ppln->vk_uniform_buffer_mapped_region_data_[i]);
			/* Persistent mapping technique the GPU memory remains mapped with the cpu memory upto the whole time of the aplication*/
		}
	}

	/* Create descriptors pool */
	{
		VkDescriptorPoolCreateInfo pool_info = { 0 };
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.poolSizeCount = _no_of_pool_sizes;
		pool_info.pPoolSizes = _pool_sizes;
		// we should also specify the max no of descriptro set that might be get allocated
		pool_info.maxSets = 5;

		if (vkCreateDescriptorPool(vk_logical_device_, &pool_info, NULL, &ppln->vk_descriptor_pool_) != VK_SUCCESS)
		{
			printf("Failed to create descriptor pool\n");
		}
	}

	/* Create descriptor Set */
	{
		VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT] = { ppln->vk_descriptor_set_layout_ , ppln->vk_descriptor_set_layout_ };
		VkDescriptorSetAllocateInfo alloc_info = { 0 };
		alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		alloc_info.descriptorPool = ppln->vk_descriptor_pool_;
		alloc_info.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
		alloc_info.pSetLayouts = layouts;

		VkResult res;
		if ((res = vkAllocateDescriptorSets(vk_logical_device_, &alloc_info, ppln->vk_descriptor_sets_)) != VK_SUCCESS)
		{
			printf("Failed to allocate descriptor sets\n");
		}

		static VkWriteDescriptorSet descriptor_writes[MAX_BUFFER_SIZE] = { 0 };
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{

			VkDescriptorBufferInfo buffer_info = { 0 };
			buffer_info.buffer = ppln->vk_uniform_buffer_[i];
			buffer_info.offset = 0;
			buffer_info.range = VK_WHOLE_SIZE;

			descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_writes[0].dstSet = ppln->vk_descriptor_sets_[i];
			descriptor_writes[0].dstBinding = 0;
			descriptor_writes[0].dstArrayElement = 0;
			descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptor_writes[0].descriptorCount = 1;
			descriptor_writes[0].pBufferInfo = &buffer_info;
			descriptor_writes[0].pImageInfo = NULL;
			descriptor_writes[0].pTexelBufferView = NULL;

		for (int j = 1; j <= _no_of_texture_images; j++)
			{
				VkDescriptorImageInfo image_info = { 0 };
				image_info = (VkDescriptorImageInfo){
					.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					.imageView = _texture_image_view[j - 1], // Yo euta matra hudaina
					.sampler = _texture_image_sampler[j - 1] // Yo euta matra hudaina
				};
				descriptor_writes[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptor_writes[j].dstSet = ppln->vk_descriptor_sets_[i];
				descriptor_writes[j].dstBinding = j;
				descriptor_writes[j].dstArrayElement = 0;
				descriptor_writes[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptor_writes[j].descriptorCount = 1;
				descriptor_writes[j].pBufferInfo = NULL;
				descriptor_writes[j].pImageInfo = &image_info;
				descriptor_writes[j].pTexelBufferView = NULL;
			}
			vkUpdateDescriptorSets(vk_logical_device_, 1 + _no_of_texture_images, descriptor_writes, 0, NULL);

		}
	}

}



void create_vulkan_pipeline2_compute(
	vk_rsrs *_rsrs,
	struct pipeline_vk* ppln,
	int _descriptor_set_layout_binding_count,
	VkDescriptorSetLayoutBinding *_descriptor_layout_binding,
	const char* _vshader_path,
	const char* _fshader_path,
	const char* _cshader_path,
	VkVertexInputBindingDescription *_vertex_input_binding_description,
	int _vertex_input_binding_description_count,
	VkVertexInputAttributeDescription *_vertex_input_attribute_description,
	int _vertex_input_attribute_description_count,
	int _no_of_texture_images,
	VkImage *_texture_images,
	VkDeviceMemory *_texture_image_memory,
	VkSampler *_texture_image_sampler,
	VkImageView *_texture_image_view,
	const char* *_texture_image_path,
	int _no_of_pool_sizes,
	VkDescriptorPoolSize *_pool_sizes
)
{
	/* Create Descriptor Set layout*/
	{

		VkDescriptorSetLayoutCreateInfo layout_info = { 0 };
		layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layout_info.bindingCount = _descriptor_set_layout_binding_count;
		layout_info.pBindings = _descriptor_layout_binding;

		if (vkCreateDescriptorSetLayout(vk_logical_device_, &layout_info, NULL, &ppln->vk_descriptor_set_layout_) != VK_SUCCESS)
		{
			printf("Error Settign descriptpor layouts\n");
		}
	}

	/* Create Graphics Pipeline */
	{
		char *vshader = NULL;
		int vshader_size = read_file_util(_vshader_path, &vshader);
		char *fshader = NULL;
		int fshader_size = read_file_util(_fshader_path, &fshader);
		char *cshader = NULL;
		int cshader_size = read_file_util(_cshader_path, &cshader);

		VkShaderModule vertex_shader_module = create_shader_module_util(vshader, vshader_size);
		VkShaderModule fragment_shader_module = create_shader_module_util(fshader, fshader_size);
		VkShaderModule compute_shader_module = create_shader_module_util(cshader, cshader_size);

		//VkPipelineShaderStageCreateInfo vertex_shader_info = { 0 };
		//vertex_shader_info = (VkPipelineShaderStageCreateInfo){
		//	.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		//	.stage = VK_SHADER_STAGE_VERTEX_BIT,
		//	.module = vertex_shader_module,
		//	.pName = "main",
		//	.pSpecializationInfo = NULL
		//	/* For compile time constants to better optimize stuffs and blah */
		//};

		//VkPipelineShaderStageCreateInfo fragment_shader_info = { 0 };
		//fragment_shader_info = (VkPipelineShaderStageCreateInfo){
		//	.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		//	.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		//	.module = fragment_shader_module,
		//	.pName = "main",
		//	.pSpecializationInfo = NULL
		//};

		VkPipelineShaderStageCreateInfo compute_shader_info = { 0 };
		compute_shader_info = (VkPipelineShaderStageCreateInfo)
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_COMPUTE_BIT,
			.module = compute_shader_module,
			.pName = "main",
			.pSpecializationInfo = NULL
		};

		//VkPipelineShaderStageCreateInfo shader_stages[] = { vertex_shader_info, fragment_shader_info };
		//VkDynamicState dynamic_states[3] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE };

		//VkPipelineDynamicStateCreateInfo dynamic_state = { 0 };
		//dynamic_state = (VkPipelineDynamicStateCreateInfo){
		//	.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		//	.dynamicStateCount = 3,
		//	.pDynamicStates = dynamic_states
		//};


		//VkPipelineVertexInputStateCreateInfo vertex_input_info = { 0 };
		//vertex_input_info = (VkPipelineVertexInputStateCreateInfo){
		//	.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		//	.vertexBindingDescriptionCount = _vertex_input_binding_description_count,
		//	.pVertexBindingDescriptions = _vertex_input_binding_description,
		//	.vertexAttributeDescriptionCount = _vertex_input_attribute_description_count,
		//	.pVertexAttributeDescriptions = _vertex_input_attribute_description
		//};

		//VkPipelineInputAssemblyStateCreateInfo input_assembly = { 0 };
		//input_assembly = (VkPipelineInputAssemblyStateCreateInfo){
		//	.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		//	.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		//	.primitiveRestartEnable = VK_FALSE,
		//};

		//VkViewport viewport = { 0 };
		//viewport = (VkViewport){
		//	.x = 0.0f,
		//	.y = 0.0f,
		//	.width = (float) _rsrs->vk_swap_chain_image_extent_2d_.width,
		//	.height = (float) _rsrs->vk_swap_chain_image_extent_2d_.height,
		//	.minDepth = 0.0f,
		//	.maxDepth = 1.0f
		//	/* SCISSOR RECTANGLES ARE WHERE THE PIXELS  ARE ACTUALLY STORED*/
		//};

		//VkRect2D scissor = { 0 };
		//scissor = (VkRect2D){
		//	.offset = (VkOffset2D){ 0, 0 },
		//	.extent = _rsrs->vk_swap_chain_image_extent_2d_
		//};

		//VkPipelineViewportStateCreateInfo viewport_state = { 0 };
		//viewport_state = (VkPipelineViewportStateCreateInfo){
		//	.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		//	.viewportCount = 1,
		//	.scissorCount = 1
		//};

		//VkPipelineRasterizationStateCreateInfo rasterizer = { 0 };
		//rasterizer = (VkPipelineRasterizationStateCreateInfo){
		//	.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		//	.depthClampEnable = VK_FALSE,
		//	.rasterizerDiscardEnable = VK_FALSE,
		//	.polygonMode = VK_POLYGON_MODE_FILL,
		//	.lineWidth = 1.0f,
		//	.cullMode = VK_CULL_MODE_FRONT_BIT,
		//	.frontFace = VK_FRONT_FACE_CLOCKWISE,
		//	.depthBiasEnable = VK_FALSE,
		//	.depthBiasConstantFactor = 0.0f,
		//		.depthBiasClamp = 0.0f,
		//		.depthBiasSlopeFactor = 0.0f
		//};

		//VkPipelineMultisampleStateCreateInfo multisampling = { 0 };
		//multisampling = (VkPipelineMultisampleStateCreateInfo){
		//	.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		//	.sampleShadingEnable = VK_FALSE,
		//	.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		//	.minSampleShading = 1.0f,
		//	.pSampleMask = NULL,
		//	.alphaToCoverageEnable = VK_FALSE,
		//	.alphaToOneEnable = VK_FALSE
		//};

		/* DEPTH AND STENCIL TESTING IS ALSO RQUIRE FOR 3D BUT NOT NOW */
		//VkPipelineColorBlendAttachmentState color_blend_attachment = { 0 };
		//color_blend_attachment = (VkPipelineColorBlendAttachmentState){
		//	.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		//	.blendEnable = VK_TRUE,
		//	.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
		//	.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		//	.colorBlendOp = VK_BLEND_OP_ADD,
		//	.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		//	.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		//	.alphaBlendOp = VK_BLEND_OP_ADD
		//};

		//VkPipelineColorBlendStateCreateInfo color_blending = { 0 };
		//color_blending = (VkPipelineColorBlendStateCreateInfo){
		//	.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		//	.logicOpEnable = VK_FALSE,
		//	.logicOp = VK_LOGIC_OP_COPY, // Optional
		//	// YAA BUJHNA BAAKI XA
		//	.attachmentCount = 1,
		//	.pAttachments = &color_blend_attachment,
		//	.blendConstants[0] = 0.0f, // Optional
		//	.blendConstants[1] = 0.0f, // Optional
		//	.blendConstants[2] = 0.0f, // Optional
		//	.blendConstants[3] = 0.0f // Optional
		//};

		/* ppln layouts for uniforms */
		VkPipelineLayoutCreateInfo pipeline_layout_info = { 0 };
		pipeline_layout_info = (VkPipelineLayoutCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = 1,
			.pSetLayouts = &ppln->vk_descriptor_set_layout_
		};

		VkPushConstantRange push_constant = { 0 };
		push_constant = (VkPushConstantRange){
			.offset = 0,
			.size = sizeof(struct push_constants),
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
		};

		pipeline_layout_info.pushConstantRangeCount = 1;
		pipeline_layout_info.pPushConstantRanges = &push_constant;
		/* pUSH CONSTANTS */

		/* Depth Stencil State */
		//VkPipelineDepthStencilStateCreateInfo depth_stencil = (VkPipelineDepthStencilStateCreateInfo)
		//{
		//	.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		//	.depthTestEnable = VK_TRUE,
		//	.depthWriteEnable = VK_TRUE,
		//	.depthCompareOp = VK_COMPARE_OP_LESS,
		//	.depthBoundsTestEnable = VK_FALSE,
		//	.minDepthBounds = 0.0f,
		//	.maxDepthBounds = 1.0f,
		//	.stencilTestEnable = VK_FALSE,
		//	.front = {0},
		//	.back = {0}
		//};

		if (vkCreatePipelineLayout(vk_logical_device_, &pipeline_layout_info, NULL, &ppln->vk_pipeline_layout_) != VK_SUCCESS)
		{
			printf("Failed to crreate ppln\n");
		}

		//VkGraphicsPipelineCreateInfo pipeline_info = { 0 };
		//pipeline_info = (VkGraphicsPipelineCreateInfo){
		//	.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		//	.stageCount = 2,
		//	.pStages = shader_stages,
		//	.pVertexInputState = &vertex_input_info,
		//	.pInputAssemblyState = &input_assembly,
		//	.pViewportState = &viewport_state,
		//	.pRasterizationState = &rasterizer,
		//	.pMultisampleState = &multisampling,
		//	.pDepthStencilState = &depth_stencil,
		//	.pColorBlendState = &color_blending,
		//	.pDynamicState = &dynamic_state,
		//	.layout = ppln->vk_pipeline_layout_,
		//	.renderPass = _rsrs->vk_render_pass_,
		//	.subpass = 0, // Index of subpas,
		//	.basePipelineHandle = VK_NULL_HANDLE,
		//	.basePipelineIndex = -1,
		//};


		VkComputePipelineCreateInfo comp_pipeline_info = (VkComputePipelineCreateInfo)
		{
			.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
			.layout = ppln->vk_pipeline_layout_,
			.stage = compute_shader_info
		};


		//if (vkCreateGraphicsPipelines(vk_logical_device_, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &ppln->vk_pipeline_) != VK_SUCCESS)
		//{
		//	printf("Failed to create graphics ppln");
		//}

		KSAI_VK_ASSERT(vkCreateComputePipelines(vk_logical_device_, VK_NULL_HANDLE, 1, &comp_pipeline_info, NULL, &ppln->vk_pipeline_));

		free(vshader);
		free(fshader);
		vkDestroyShaderModule(vk_logical_device_, vertex_shader_module, NULL);
		vkDestroyShaderModule(vk_logical_device_, fragment_shader_module, NULL);
	}

	/* Texture Image */
	for (int i = 0; i < _no_of_texture_images; i++)
	{
		/* Create Texture Image */
		{
			int tex_width, tex_height, tex_channels;
			stbi_uc* pixels;
			stbi_set_flip_vertically_on_load(true);
			if(strcmp(_texture_image_path[i], "") == 0)
				pixels = stbi_load("res/textures/checker.png", &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
			else
				pixels = stbi_load(_texture_image_path[i], &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

			VkDeviceSize image_size = tex_width * tex_height * 4;
			if (!pixels)
				printf("error, cannot load image_temporary_testing_ \n");
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

			void* data;
			vkMapMemory(vk_logical_device_, staging_buffer_memory, 0, image_size, 0, &data);
			memcpy(data, pixels, (size_t)image_size);
			vkUnmapMemory(vk_logical_device_, staging_buffer_memory);
			stbi_image_free(pixels);
			create_image_util(tex_width, tex_height,
				VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&_texture_images[i],
				&_texture_image_memory[i],
				vk_logical_device_
			);
			transition_image_layout_util(
				_texture_images[i],
				VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				vk_command_pool_,
				_rsrs->vk_graphics_queue_
			);
			copy_buffer_to_image_util(
				staging_buffer,
				_texture_images[i],
				tex_width,
				tex_height,
				vk_command_pool_,
				_rsrs->vk_graphics_queue_
			);
			transition_image_layout_util(
				_texture_images[i],
				VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				vk_command_pool_,
				_rsrs->vk_graphics_queue_
			);
			vkDeviceWaitIdle(vk_logical_device_);
			vkDestroyBuffer(vk_logical_device_, staging_buffer, NULL);
			vkFreeMemory(vk_logical_device_, staging_buffer_memory, NULL);

			VkImageViewCreateInfo create_info = { 0 };
			create_info = (VkImageViewCreateInfo)
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = _texture_images[i],
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = VK_FORMAT_R8G8B8A8_SRGB,
				.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.subresourceRange.baseMipLevel = 0,
				.subresourceRange.levelCount = 1,
				.subresourceRange.baseArrayLayer = 0,
				.subresourceRange.layerCount = 1,
				.components = {VK_COMPONENT_SWIZZLE_IDENTITY}
			};

			if (vkCreateImageView(vk_logical_device_, &create_info, NULL, &_texture_image_view[i]) != VK_SUCCESS)
			{
				printf("Failed to create image_temporary_testing_ views \n");
			}

			/* Create texture Sampler */
			{
				create_texture_sampler(
					&_texture_image_sampler[i],
					vk_logical_device_,
					vk_physical_device_
				);
			}
		}
	}

	/*Create Uniform buffers*/
	{
		VkDeviceSize buffer_size = sizeof(ppln->ubo);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			/* Create the Actual Uniform Buffer */
			{
				VkBufferCreateInfo create_info = { 0 };
				create_info = (VkBufferCreateInfo)
				{
					.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
					.size = buffer_size,
					.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					.sharingMode = VK_SHARING_MODE_EXCLUSIVE
				};

				if (vkCreateBuffer(vk_logical_device_, &create_info, NULL, &ppln->vk_uniform_buffer_[i]) != VK_SUCCESS)
				{
					printf("Failed to create vertex Buffers\n");
				}

				/*Allocate the memory*/
				{
					VkMemoryRequirements memory_requirements = { 0 };
					vkGetBufferMemoryRequirements(vk_logical_device_, ppln->vk_uniform_buffer_[i], &memory_requirements);

					VkMemoryAllocateInfo alloc_info = { 0 };
					alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
					alloc_info.allocationSize = memory_requirements.size;
					alloc_info.memoryTypeIndex = find_memory_type_util(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
					/* yo chae mazzale nai bujhna baaki xa */

					if (vkAllocateMemory(vk_logical_device_, &alloc_info, NULL, &ppln->vk_uniform_buffer_memory_[i]) != VK_SUCCESS)
					{
						printf("Failed to Allocate Vertex Buffer Memory\n");
					}
					// Allocate matrai haina, bind ni garnu paro buffer sanga, buffer ko memory chae yo ho hai vnna paron ta lul
					vkBindBufferMemory(vk_logical_device_, ppln->vk_uniform_buffer_[i], ppln->vk_uniform_buffer_memory_[i], 0);
				}
			}
			vkMapMemory(vk_logical_device_, ppln->vk_uniform_buffer_memory_[i], 0, buffer_size, 0, &ppln->vk_uniform_buffer_mapped_region_data_[i]);
			/* Persistent mapping technique the GPU memory remains mapped with the cpu memory upto the whole time of the aplication*/
		}
	}

	/* Create descriptors pool */
	{
		VkDescriptorPoolCreateInfo pool_info = { 0 };
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.poolSizeCount = _no_of_pool_sizes;
		pool_info.pPoolSizes = _pool_sizes;
		// we should also specify the max no of descriptro set that might be get allocated
		pool_info.maxSets = MAX_FRAMES_IN_FLIGHT;

		if (vkCreateDescriptorPool(vk_logical_device_, &pool_info, NULL, &ppln->vk_descriptor_pool_) != VK_SUCCESS)
		{
			printf("Failed to create descriptor pool\n");
		}
	}

	/* Create descriptor Set */
	{
		VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT] = { ppln->vk_descriptor_set_layout_ , ppln->vk_descriptor_set_layout_ };
		VkDescriptorSetAllocateInfo alloc_info = { 0 };
		alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		alloc_info.descriptorPool = ppln->vk_descriptor_pool_;
		alloc_info.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
		alloc_info.pSetLayouts = layouts;

		VkResult res;
		if ((res = vkAllocateDescriptorSets(vk_logical_device_, &alloc_info, ppln->vk_descriptor_sets_)) != VK_SUCCESS)
		{
			printf("Failed to allocate descriptor sets\n");
		}

		static VkWriteDescriptorSet descriptor_writes[MAX_BUFFER_SIZE] = { 0 };
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{

			VkDescriptorBufferInfo buffer_info = { 0 };
			buffer_info.buffer = ppln->vk_uniform_buffer_[i];
			buffer_info.offset = 0;
			buffer_info.range = VK_WHOLE_SIZE;

			descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_writes[0].dstSet = ppln->vk_descriptor_sets_[i];
			descriptor_writes[0].dstBinding = 0;
			descriptor_writes[0].dstArrayElement = 0;
			descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptor_writes[0].descriptorCount = 1;
			descriptor_writes[0].pBufferInfo = &buffer_info;
			descriptor_writes[0].pImageInfo = NULL;
			descriptor_writes[0].pTexelBufferView = NULL;

	for (int j = 1; j <= _no_of_texture_images; j++)
			{
				VkDescriptorImageInfo image_info = { 0 };
				image_info = (VkDescriptorImageInfo){
					.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					.imageView = _texture_image_view[j - 1], // Yo euta matra hudaina
					.sampler = _texture_image_sampler[j - 1] // Yo euta matra hudaina
				};
				descriptor_writes[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptor_writes[j].dstSet = ppln->vk_descriptor_sets_[i];
				descriptor_writes[j].dstBinding = j;
				descriptor_writes[j].dstArrayElement = 0;
				descriptor_writes[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptor_writes[j].descriptorCount = 1;
				descriptor_writes[j].pBufferInfo = NULL;
				descriptor_writes[j].pImageInfo = &image_info;
				descriptor_writes[j].pTexelBufferView = NULL;
			}
			vkUpdateDescriptorSets(vk_logical_device_, 1 + _no_of_texture_images, descriptor_writes, 0, NULL);

		}
	}

}

void pipeline_vk_destroy(pipeline_vk *_ppln)
{
	if(_ppln->vk_vertex_buffer_ != 0)
	{ 
		vkDestroyBuffer(vk_logical_device_, _ppln->vk_vertex_buffer_, NULL);
		vkFreeMemory(vk_logical_device_, _ppln->vk_vertex_buffer_memory_, NULL);
	}

	if(_ppln->vk_index_buffer_ != 0)
	{ 
		vkDestroyBuffer(vk_logical_device_, _ppln->vk_index_buffer_, NULL);
		vkFreeMemory(vk_logical_device_, _ppln->vk_index_buffer_memory_, NULL);
	}
	if (_ppln->vk_texture_image_ != 0)
	{
		vkDestroyImage(vk_logical_device_, _ppln->vk_texture_image_, NULL);
		vkDestroyImageView(vk_logical_device_, _ppln->vk_texture_image_view_, NULL);
		vkDestroySampler(vk_logical_device_, _ppln->vk_texture_image_sampler_, NULL);
		vkFreeMemory(vk_logical_device_, _ppln->vk_texture_image_memory_, NULL);
	}
	for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{ 
		vkUnmapMemory(vk_logical_device_, _ppln->vk_uniform_buffer_memory_[i]);
		vkDestroyBuffer(vk_logical_device_, _ppln->vk_uniform_buffer_[i], NULL);
		vkFreeMemory(vk_logical_device_, _ppln->vk_uniform_buffer_memory_[i], NULL);
	}
	vkDestroyDescriptorSetLayout(vk_logical_device_, _ppln->vk_descriptor_set_layout_, NULL);
	vkDestroyDescriptorPool(vk_logical_device_, _ppln->vk_descriptor_pool_, NULL);
	vkDestroyPipelineLayout(vk_logical_device_, _ppln->vk_pipeline_layout_, NULL);
	vkDestroyPipeline(vk_logical_device_, _ppln->vk_pipeline_, NULL);

}

void pipeline_vk_destroy2(pipeline_vk *_ppln)
{
	if (_ppln->vk_texture_image_ != 0)
	{
		vkDestroyImage(vk_logical_device_, _ppln->vk_texture_image_, NULL);
		vkDestroyImageView(vk_logical_device_, _ppln->vk_texture_image_view_, NULL);
		vkDestroySampler(vk_logical_device_, _ppln->vk_texture_image_sampler_, NULL);
		vkFreeMemory(vk_logical_device_, _ppln->vk_texture_image_memory_, NULL);
	}
	for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{ 
		vkUnmapMemory(vk_logical_device_, _ppln->vk_uniform_buffer_memory_[i]);
		vkDestroyBuffer(vk_logical_device_, _ppln->vk_uniform_buffer_[i], NULL);
		vkFreeMemory(vk_logical_device_, _ppln->vk_uniform_buffer_memory_[i], NULL);
	}
	vkDestroyDescriptorSetLayout(vk_logical_device_, _ppln->vk_descriptor_set_layout_, NULL);
	vkDestroyDescriptorPool(vk_logical_device_, _ppln->vk_descriptor_pool_, NULL);
	vkDestroyPipelineLayout(vk_logical_device_, _ppln->vk_pipeline_layout_, NULL);
	vkDestroyPipeline(vk_logical_device_, _ppln->vk_pipeline_, NULL);

}

void create_vulkan_pipeline3(
	vk_rsrs *_rsrs,
	struct pipeline_vk* ppln,
	int _descriptor_set_layout_binding_count,
	VkDescriptorSetLayoutBinding *_descriptor_layout_binding,
	const char* _vshader_path,
	const char* _fshader_path,
	VkVertexInputBindingDescription *_vertex_input_binding_description,
	int _vertex_input_binding_description_count,
	VkVertexInputAttributeDescription *_vertex_input_attribute_description,
	int _vertex_input_attribute_description_count,
	int _no_of_pool_sizes,
	VkDescriptorPoolSize *_pool_sizes
)
{
	/* Create Descriptor Set layout*/
	{
		VkDescriptorSetLayoutCreateInfo layout_info = { 0 };
		layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layout_info.bindingCount = _descriptor_set_layout_binding_count;
		layout_info.pBindings = _descriptor_layout_binding;

		if (vkCreateDescriptorSetLayout(vk_logical_device_, &layout_info, NULL, &ppln->vk_descriptor_set_layout_) != VK_SUCCESS)
		{
			printf("Error Settign descriptpor layouts\n");
		}
	}

	/* Create Graphics Pipeline */
	{
		char *vshader = NULL;
		int vshader_size = read_file_util(_vshader_path, &vshader);
		char *fshader = NULL;
		int fshader_size = read_file_util(_fshader_path, &fshader);

		VkShaderModule vertex_shader_module = create_shader_module_util(vshader, vshader_size);
		VkShaderModule fragment_shader_module = create_shader_module_util(fshader, fshader_size);

		VkPipelineShaderStageCreateInfo vertex_shader_info = { 0 };
		vertex_shader_info = (VkPipelineShaderStageCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = vertex_shader_module,
			.pName = "main",
			.pSpecializationInfo = NULL
			/* For compile time constants to better optimize stuffs and blah */
		};

		VkPipelineShaderStageCreateInfo fragment_shader_info = { 0 };
		fragment_shader_info = (VkPipelineShaderStageCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = fragment_shader_module,
			.pName = "main",
			.pSpecializationInfo = NULL
		};

		VkPipelineShaderStageCreateInfo shader_stages[] = { vertex_shader_info, fragment_shader_info };
		VkDynamicState dynamic_states[3] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE };

		VkPipelineDynamicStateCreateInfo dynamic_state = { 0 };
		dynamic_state = (VkPipelineDynamicStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = 3,
			.pDynamicStates = dynamic_states
		};


		VkPipelineVertexInputStateCreateInfo vertex_input_info = { 0 };
		vertex_input_info = (VkPipelineVertexInputStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = _vertex_input_binding_description_count,
			.pVertexBindingDescriptions = _vertex_input_binding_description,
			.vertexAttributeDescriptionCount = _vertex_input_attribute_description_count,
			.pVertexAttributeDescriptions = _vertex_input_attribute_description
		};

		VkPipelineInputAssemblyStateCreateInfo input_assembly = { 0 };
		input_assembly = (VkPipelineInputAssemblyStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE,
		};


		VkPipelineViewportStateCreateInfo viewport_state = { 0 };
		viewport_state = (VkPipelineViewportStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.scissorCount = 1
		};

		VkPipelineRasterizationStateCreateInfo rasterizer = { 0 };
		rasterizer = (VkPipelineRasterizationStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = VK_POLYGON_MODE_FILL,
			.lineWidth = 1.0f,
			.cullMode = VK_CULL_MODE_NONE,
			.frontFace = VK_FRONT_FACE_CLOCKWISE,
			.depthBiasEnable = VK_FALSE,
			.depthBiasConstantFactor = 0.0f,
				.depthBiasClamp = 0.0f,
				.depthBiasSlopeFactor = 0.0f
		};

		VkPipelineMultisampleStateCreateInfo multisampling = { 0 };
		multisampling = (VkPipelineMultisampleStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.sampleShadingEnable = VK_FALSE,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.minSampleShading = 1.0f,
			.pSampleMask = NULL,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE
		};

		/* DEPTH AND STENCIL TESTING IS ALSO RQUIRE FOR 3D BUT NOT NOW */
		VkPipelineColorBlendAttachmentState color_blend_attachment = { 0 };
		color_blend_attachment = (VkPipelineColorBlendAttachmentState){
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
			.blendEnable = VK_TRUE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
			.alphaBlendOp = VK_BLEND_OP_ADD
		};

		VkPipelineColorBlendStateCreateInfo color_blending = { 0 };
		color_blending = (VkPipelineColorBlendStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY, // Optional
			// YAA BUJHNA BAAKI XA
			.attachmentCount = 1,
			.pAttachments = &color_blend_attachment,
			.blendConstants[0] = 0.0f, // Optional
			.blendConstants[1] = 0.0f, // Optional
			.blendConstants[2] = 0.0f, // Optional
			.blendConstants[3] = 0.0f // Optional
		};

		/* ppln layouts for uniforms */
		VkPipelineLayoutCreateInfo pipeline_layout_info = { 0 };
		pipeline_layout_info = (VkPipelineLayoutCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = 1,
			.pSetLayouts = &ppln->vk_descriptor_set_layout_
		};

		VkPushConstantRange push_constant = { 0 };
		push_constant = (VkPushConstantRange){
			.offset = 0,
			.size = sizeof(struct push_constants),
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
		};

		pipeline_layout_info.pushConstantRangeCount = 1;
		pipeline_layout_info.pPushConstantRanges = &push_constant;
		/* pUSH CONSTANTS */

		/* Depth Stencil State */
		VkPipelineDepthStencilStateCreateInfo depth_stencil = (VkPipelineDepthStencilStateCreateInfo)
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable = VK_TRUE,
			.depthWriteEnable = VK_TRUE,
			.depthCompareOp = VK_COMPARE_OP_LESS,
			.depthBoundsTestEnable = VK_FALSE,
			.minDepthBounds = 0.0f,
			.maxDepthBounds = 1.0f,
			.stencilTestEnable = VK_FALSE,
			.front = {0},
			.back = {0}
		};

		if (vkCreatePipelineLayout(vk_logical_device_, &pipeline_layout_info, NULL, &ppln->vk_pipeline_layout_) != VK_SUCCESS)
		{
			printf("Failed to crreate ppln\n");
		}

		VkGraphicsPipelineCreateInfo pipeline_info = { 0 };
		pipeline_info = (VkGraphicsPipelineCreateInfo){
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.stageCount = 2,
			.pStages = shader_stages,
			.pVertexInputState = &vertex_input_info,
			.pInputAssemblyState = &input_assembly,
			.pViewportState = &viewport_state,
			.pRasterizationState = &rasterizer,
			.pMultisampleState = &multisampling,
			.pDepthStencilState = &depth_stencil,
			.pColorBlendState = &color_blending,
			.pDynamicState = &dynamic_state,
			.layout = ppln->vk_pipeline_layout_,
			.renderPass = _rsrs->vk_render_pass_,
			.subpass = 0, // Index of subpas,
			.basePipelineHandle = VK_NULL_HANDLE,
			.basePipelineIndex = -1,
		};

		if (vkCreateGraphicsPipelines(vk_logical_device_, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &ppln->vk_pipeline_) != VK_SUCCESS)
		{
			printf("Failed to create graphics ppln");
		}

		free(vshader);
		free(fshader);
		vkDestroyShaderModule(vk_logical_device_, vertex_shader_module, NULL);
		vkDestroyShaderModule(vk_logical_device_, fragment_shader_module, NULL);
	}

}

void create_vulkan_pipeline3_skybox(
	vk_rsrs *_rsrs,
	struct pipeline_vk* ppln,
	int _descriptor_set_layout_binding_count,
	VkDescriptorSetLayoutBinding *_descriptor_layout_binding,
	const char* _vshader_path,
	const char* _fshader_path,
	VkVertexInputBindingDescription *_vertex_input_binding_description,
	int _vertex_input_binding_description_count,
	VkVertexInputAttributeDescription *_vertex_input_attribute_description,
	int _vertex_input_attribute_description_count,
	int _no_of_pool_sizes,
	VkDescriptorPoolSize *_pool_sizes
)
{
	/* Create Descriptor Set layout*/
	{
		VkDescriptorSetLayoutCreateInfo layout_info = { 0 };
		layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layout_info.bindingCount = _descriptor_set_layout_binding_count;
		layout_info.pBindings = _descriptor_layout_binding;

		if (vkCreateDescriptorSetLayout(vk_logical_device_, &layout_info, NULL, &ppln->vk_descriptor_set_layout_) != VK_SUCCESS)
		{
			printf("Error Settign descriptpor layouts\n");
		}
	}

	/* Create Graphics Pipeline */
	{
		char *vshader = NULL;
		int vshader_size = read_file_util(_vshader_path, &vshader);
		char *fshader = NULL;
		int fshader_size = read_file_util(_fshader_path, &fshader);

		VkShaderModule vertex_shader_module = create_shader_module_util(vshader, vshader_size);
		VkShaderModule fragment_shader_module = create_shader_module_util(fshader, fshader_size);

		VkPipelineShaderStageCreateInfo vertex_shader_info = { 0 };
		vertex_shader_info = (VkPipelineShaderStageCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = vertex_shader_module,
			.pName = "main",
			.pSpecializationInfo = NULL
			/* For compile time constants to better optimize stuffs and blah */
		};

		VkPipelineShaderStageCreateInfo fragment_shader_info = { 0 };
		fragment_shader_info = (VkPipelineShaderStageCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = fragment_shader_module,
			.pName = "main",
			.pSpecializationInfo = NULL
		};

		VkPipelineShaderStageCreateInfo shader_stages[] = { vertex_shader_info, fragment_shader_info };
		VkDynamicState dynamic_states[3] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE };

		VkPipelineDynamicStateCreateInfo dynamic_state = { 0 };
		dynamic_state = (VkPipelineDynamicStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = 3,
			.pDynamicStates = dynamic_states
		};


		VkPipelineVertexInputStateCreateInfo vertex_input_info = { 0 };
		vertex_input_info = (VkPipelineVertexInputStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = _vertex_input_binding_description_count,
			.pVertexBindingDescriptions = _vertex_input_binding_description,
			.vertexAttributeDescriptionCount = _vertex_input_attribute_description_count,
			.pVertexAttributeDescriptions = _vertex_input_attribute_description
		};

		VkPipelineInputAssemblyStateCreateInfo input_assembly = { 0 };
		input_assembly = (VkPipelineInputAssemblyStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE,
		};


		VkPipelineViewportStateCreateInfo viewport_state = { 0 };
		viewport_state = (VkPipelineViewportStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.scissorCount = 1
		};

		VkPipelineRasterizationStateCreateInfo rasterizer = { 0 };
		rasterizer = (VkPipelineRasterizationStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = VK_POLYGON_MODE_FILL,
			.lineWidth = 1.0f,
			.cullMode = VK_CULL_MODE_FRONT_BIT,
			.frontFace = VK_FRONT_FACE_CLOCKWISE,
			.depthBiasEnable = VK_FALSE,
			.depthBiasConstantFactor = 0.0f,
				.depthBiasClamp = 0.0f,
				.depthBiasSlopeFactor = 0.0f
		};

		VkPipelineMultisampleStateCreateInfo multisampling = { 0 };
		multisampling = (VkPipelineMultisampleStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.sampleShadingEnable = VK_FALSE,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.minSampleShading = 1.0f,
			.pSampleMask = NULL,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE
		};

		/* DEPTH AND STENCIL TESTING IS ALSO RQUIRE FOR 3D BUT NOT NOW */
		VkPipelineColorBlendAttachmentState color_blend_attachment = { 0 };
		color_blend_attachment = (VkPipelineColorBlendAttachmentState){
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
			.blendEnable = VK_FALSE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
			.alphaBlendOp = VK_BLEND_OP_ADD
		};

		VkPipelineColorBlendStateCreateInfo color_blending = { 0 };
		color_blending = (VkPipelineColorBlendStateCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY, // Optional
			// YAA BUJHNA BAAKI XA
			.attachmentCount = 1,
			.pAttachments = &color_blend_attachment,
			.blendConstants[0] = 0.0f, // Optional
			.blendConstants[1] = 0.0f, // Optional
			.blendConstants[2] = 0.0f, // Optional
			.blendConstants[3] = 0.0f // Optional
		};

		/* ppln layouts for uniforms */
		VkPipelineLayoutCreateInfo pipeline_layout_info = { 0 };
		pipeline_layout_info = (VkPipelineLayoutCreateInfo){
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = 1,
			.pSetLayouts = &ppln->vk_descriptor_set_layout_
		};

		VkPushConstantRange push_constant = { 0 };
		push_constant = (VkPushConstantRange){
			.offset = 0,
			.size = sizeof(struct push_constants),
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
		};

		pipeline_layout_info.pushConstantRangeCount = 1;
		pipeline_layout_info.pPushConstantRanges = &push_constant;
		/* pUSH CONSTANTS */

		/* Depth Stencil State */
		VkPipelineDepthStencilStateCreateInfo depth_stencil = (VkPipelineDepthStencilStateCreateInfo)
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable = VK_TRUE,
			.depthWriteEnable = VK_TRUE,
			.depthCompareOp = VK_COMPARE_OP_LESS,
			.depthBoundsTestEnable = VK_FALSE,
			.minDepthBounds = 0.0f,
			.maxDepthBounds = 1.0f,
			.stencilTestEnable = VK_FALSE,
			.front = {0},
			.back = {0}
		};

		if (vkCreatePipelineLayout(vk_logical_device_, &pipeline_layout_info, NULL, &ppln->vk_pipeline_layout_) != VK_SUCCESS)
		{
			printf("Failed to crreate ppln\n");
		}

		VkGraphicsPipelineCreateInfo pipeline_info = { 0 };
		pipeline_info = (VkGraphicsPipelineCreateInfo){
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.stageCount = 2,
			.pStages = shader_stages,
			.pVertexInputState = &vertex_input_info,
			.pInputAssemblyState = &input_assembly,
			.pViewportState = &viewport_state,
			.pRasterizationState = &rasterizer,
			.pMultisampleState = &multisampling,
			.pDepthStencilState = &depth_stencil,
			.pColorBlendState = &color_blending,
			.pDynamicState = &dynamic_state,
			.layout = ppln->vk_pipeline_layout_,
			.renderPass = _rsrs->vk_render_pass_,
			.subpass = 0, // Index of subpas,
			.basePipelineHandle = VK_NULL_HANDLE,
			.basePipelineIndex = -1,
		};

		if (vkCreateGraphicsPipelines(vk_logical_device_, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &ppln->vk_pipeline_) != VK_SUCCESS)
		{
			printf("Failed to create graphics ppln");
		}

		free(vshader);
		free(fshader);
		vkDestroyShaderModule(vk_logical_device_, vertex_shader_module, NULL);
		vkDestroyShaderModule(vk_logical_device_, fragment_shader_module, NULL);
	}

}

void pipeline_vk_destroy3(pipeline_vk *_ppln)
{
	vkDestroyDescriptorSetLayout(vk_logical_device_, _ppln->vk_descriptor_set_layout_, NULL);
	vkDestroyPipelineLayout(vk_logical_device_, _ppln->vk_pipeline_layout_, NULL);
	vkDestroyPipeline(vk_logical_device_, _ppln->vk_pipeline_, NULL);

}
