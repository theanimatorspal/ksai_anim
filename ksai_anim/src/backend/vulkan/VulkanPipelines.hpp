#pragma once
extern "C" {
#include "backend/vulkan/backend.h"
#include "backend/vulkan/init.h"
}

namespace VulkanPipeline
{
    void createPipelineForShadow(const vk_rsrs *rsrs, renderer_backend *backend, pipeline_vk *inPipeline, VkRenderPass rn_pass);
    void createPipelineForPP(const vk_rsrs* rsrs, renderer_backend* backend, pipeline_vk* inPipeline, VkRenderPass rn_pass);
}

