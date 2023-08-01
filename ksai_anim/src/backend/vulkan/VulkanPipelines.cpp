#include <vulkan/vulkan.hpp>

#include "engine/objects/object.h"
#include "backend/vulkan/utils.h"
#include "backend/vulkan/backend.h"


extern "C" auto createPipelineForShadow(vk_rsrs *rsrs, renderer_backend *backend, pipeline_vk *inPipeline)
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
		.offset = static_cast<uint32_t>(offsetof(kie_Vertex, position))
	};

	attr_desp[1] = (VkVertexInputAttributeDescription){
		.binding = 0,
		.location = 1,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = static_cast<uint32_t>(offsetof(kie_Vertex, normal))
	};

	attr_desp[2] = (VkVertexInputAttributeDescription){
		.binding = 0,
		.location = 2,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = static_cast<uint32_t>(offsetof(kie_Vertex, color))
	};

	attr_desp[3] = (VkVertexInputAttributeDescription){
		.binding = 0,
		.location = 3,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = static_cast<uint32_t>(offsetof(kie_Vertex, tangent))
	};

	attr_desp[4] = (VkVertexInputAttributeDescription){
		.binding = 0,
		.location = 4,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = static_cast<uint32_t>(offsetof(kie_Vertex, bit_tangent))
	};

	attr_desp[5] = (VkVertexInputAttributeDescription){
		.binding = 0,
		.location = 5,
		.format = VK_FORMAT_R32G32_SFLOAT,
		.offset = static_cast<uint32_t>(offsetof(kie_Vertex, tex_coord))
	};


	VkDescriptorPoolSize pool_sizes[5] =  {  };
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
		rsrs,
		inPipeline,
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
