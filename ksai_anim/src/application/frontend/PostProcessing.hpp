#include "app_ui.h"

extern void PrepareForShadows(const vk_rsrs* rsrs, renderer_backend* inBackend);
extern void DestroyForShadows(vk_rsrs* rsrs, renderer_backend* backend);
extern void DrawShadows(kie_Camera* camera, kie_Scene* scene, uint32_t viewport_obj_count, vk_rsrs* rsrs, renderer_backend* backend, VkCommandBuffer inCmdBuffer);
extern void PrepareForPostProcessing(const vk_rsrs* rsrs, renderer_backend* inBackend);
extern void DestroyForPostProcessing(const vk_rsrs* rsr, renderer_backend* backend);
