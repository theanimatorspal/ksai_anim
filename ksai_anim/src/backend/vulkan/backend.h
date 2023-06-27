#pragma once
#include "init.h"
#include <engine/renderer/scene.h>
#include "pipelines.h"

typedef struct renderer_backend
{
	pipeline_vk checker_pipeline;
	pipeline_vk ksai_render_pipeline;
	VkBuffer vbuffer;
	VkDeviceMemory vbuffer_memory;
	VkDeviceSize voffset;
	VkBuffer ibuffer;
	VkDeviceMemory ibuffer_memory;
	VkDeviceSize ioffset;

	VkDeviceSize *voffsets;
	VkDeviceSize *ioffsets;
	uint32_t offset_count;

} renderer_backend;

KSAI_API void copy_scene_to_backend(vk_rsrs *rsrs, kie_Scene *scene, renderer_backend *backend);

KSAI_API void initialize_backend(vk_rsrs *rsrs, VkInstance *instance);
KSAI_API void destroy_backend(vk_rsrs *_rsrs);

KSAI_API void initialize_renderer_backend(vk_rsrs *rsrs, renderer_backend *backend);
KSAI_API void destroy_renderer_backend(vk_rsrs *rsrs, renderer_backend *backend);


KSAI_API int draw_backend_start(vk_rsrs *_rsrs);
KSAI_API int draw_backend_begin(vk_rsrs *_rsrs, vec3 color);
KSAI_API void draw_backend_end(vk_rsrs *_rsrs);
KSAI_API void draw_backend_finish(vk_rsrs *_rsrs);
KSAI_API void draw_backend_wait(vk_rsrs *_rsrs);


