#include "app_ui.h"

extern	void CreateExternalWindow(vk_rsrs* rsrs, VkInstance inInstance);
extern	void ShowExternalWindow();
extern	void HideExternalWindow();
extern void PresentToExternalWindow(kie_Camera* camera, kie_Scene* scene, uint32_t viewport_obj_count, vk_rsrs* rsrs, renderer_backend* backend, int* inCurrentPipeline);
extern void DestroyExternalWindow(VkInstance inInstance);
