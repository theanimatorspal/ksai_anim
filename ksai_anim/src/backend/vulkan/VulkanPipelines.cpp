#include <vulkan/vulkan.hpp>
#include "VulkanPipelines.hpp"

extern "C" {
    #include "engine/objects/object.h"
    #include "backend/vulkan/utils.h"
    #include "backend/vulkan/backend.h"
}

namespace VulkanPipelines
{
    void PipelineForShadow(
        const vk_rsrs *_rsrs,
        VkRenderPass rn_pass,
        struct pipeline_vk *ppln,
        int _descriptor_set_layout_binding_count,
        VkDescriptorSetLayoutBinding *_descriptor_layout_binding,
        const char *_vshader_path,
        const char *_fshader_path,
        VkVertexInputBindingDescription *_vertex_input_binding_description,
        uint32_t _vertex_input_binding_description_count,
        VkVertexInputAttributeDescription *_vertex_input_attribute_description,
        uint32_t _vertex_input_attribute_description_count,
        uint32_t _no_of_pool_sizes,
        VkDescriptorPoolSize *_pool_sizes
    )
    {
        /* Create Descriptor Set layout*/
        {
            VkDescriptorSetLayoutCreateInfo layout_info = {  };
            layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layout_info.bindingCount = _descriptor_set_layout_binding_count;
            layout_info.pBindings = _descriptor_layout_binding;

            if (vkCreateDescriptorSetLayout(vk_logical_device_, &layout_info, nullptr, &ppln->vk_descriptor_set_layout_) != VK_SUCCESS)
            {
                printf("Error Settign descriptpor layouts\n");
            }
        }

        /* Create Graphics Pipeline */
        {
            char *vshader = NULL;
            int vshader_size = read_file_util(_vshader_path, &vshader);

            VkShaderModule vertex_shader_module = create_shader_module_util(vshader, vshader_size);

            VkPipelineShaderStageCreateInfo vertex_shader_info = {  };
            vertex_shader_info = (VkPipelineShaderStageCreateInfo){
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertex_shader_module,
                .pName = "main",
                .pSpecializationInfo = nullptr
                /* For compile time constants to better optimize stuffs and blah */
            };

            VkPipelineShaderStageCreateInfo shader_stages[] = { vertex_shader_info };
            VkDynamicState dynamic_states[3] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE };

            VkPipelineDynamicStateCreateInfo dynamic_state = {  };
            dynamic_state = (VkPipelineDynamicStateCreateInfo){
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                .dynamicStateCount = 3,
                .pDynamicStates = dynamic_states
            };


            VkPipelineVertexInputStateCreateInfo vertex_input_info = {  };
            vertex_input_info = (VkPipelineVertexInputStateCreateInfo){
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                .vertexBindingDescriptionCount = _vertex_input_binding_description_count,
                .pVertexBindingDescriptions = _vertex_input_binding_description,
                .vertexAttributeDescriptionCount = _vertex_input_attribute_description_count,
                .pVertexAttributeDescriptions = _vertex_input_attribute_description
            };

            VkPipelineInputAssemblyStateCreateInfo input_assembly = {  };
            input_assembly = (VkPipelineInputAssemblyStateCreateInfo){
                .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                .primitiveRestartEnable = VK_FALSE,
            };


            VkPipelineViewportStateCreateInfo viewport_state = {  };
            viewport_state = (VkPipelineViewportStateCreateInfo){
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                .viewportCount = 1,
                .scissorCount = 1
            };

            VkPipelineRasterizationStateCreateInfo rasterizer = {  };
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

            VkPipelineMultisampleStateCreateInfo multisampling = {  };
            multisampling = (VkPipelineMultisampleStateCreateInfo){
                .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                .sampleShadingEnable = VK_FALSE,
                .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                .minSampleShading = 1.0f,
                .pSampleMask = nullptr,
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

            VkPipelineColorBlendStateCreateInfo color_blending = {  };
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
            VkPipelineLayoutCreateInfo pipeline_layout_info = {  };
            pipeline_layout_info = (VkPipelineLayoutCreateInfo){
                .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                .setLayoutCount = 1,
                .pSetLayouts = &ppln->vk_descriptor_set_layout_
            };

            VkPushConstantRange push_constant = { 0 };
            push_constant = (VkPushConstantRange){
                .offset = 0,
                .size = sizeof(struct push_constants),
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
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
                //.front = {0},
                //.back = {0}
            };

            if (vkCreatePipelineLayout(vk_logical_device_, &pipeline_layout_info, NULL, &ppln->vk_pipeline_layout_) != VK_SUCCESS)
            {
                printf("Failed to crreate ppln\n");
            }

            VkGraphicsPipelineCreateInfo pipeline_info = {  };
            pipeline_info = (VkGraphicsPipelineCreateInfo){
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .stageCount = 1,
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
                .renderPass = rn_pass,
                .subpass = 0, // Index of subpas,
                .basePipelineHandle = VK_NULL_HANDLE,
                .basePipelineIndex = -1,
            };

            if (vkCreateGraphicsPipelines(vk_logical_device_, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &ppln->vk_pipeline_) != VK_SUCCESS)
            {
                printf("Failed to create graphics ppln");
            }

            free(vshader);
            vkDestroyShaderModule(vk_logical_device_, vertex_shader_module, nullptr);
        }

    }
}



void VulkanPipeline::createPipelineForShadow(const vk_rsrs *rsrs, renderer_backend *backend, pipeline_vk *inPipeline, VkRenderPass rn_pass)
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


    VkDescriptorPoolSize pool_sizes[5] = {  };
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

    VulkanPipelines::PipelineForShadow(
        rsrs,
        rn_pass,
        inPipeline,
        KSAI_VK_DESCRIPTOR_POOL_SIZE,
        bindings,
        "res/shaders/renderer/shadow/vshader.spv",
        "res/shaders/renderer/shadow/fshader.spv",
        &binding_desp,
        1,
        attr_desp,
        6,
        KSAI_VK_DESCRIPTOR_POOL_SIZE,
        backend->pool_sizes
    );
}

void VulkanPipeline::createPipelineForPP(const vk_rsrs *rsrs, renderer_backend *backend, pipeline_vk *inPipeline, VkRenderPass rn_pass)
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


    VkDescriptorPoolSize pool_sizes[5] = {  };
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
        (vk_rsrs *)rsrs,
        inPipeline,
        KSAI_VK_DESCRIPTOR_POOL_SIZE,
        bindings,
        "res/shaders/renderer/post_process/vshader.spv",
        "res/shaders/renderer/post_process/fshader.spv",
        &binding_desp,
        1,
        attr_desp,
        6,
        KSAI_VK_DESCRIPTOR_POOL_SIZE,
        backend->pool_sizes
    );
}
