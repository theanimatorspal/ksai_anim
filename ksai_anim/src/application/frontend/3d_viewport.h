#pragma once
#include <engine/objects/object.h>
#include <engine/renderer/scene.h>
#include <backend/vulkan/backend.h>
#include <backend/vulkan/offscreen.h>
#include <backend/vulkan/advanced.h>
#include <vendor/cglm/cglm.h>
#include <vendor/stbi/stbi_image_write.h>
#include <SDL2/SDL.h>


void threeD_viewport_init(kie_Camera *camera, int args, ...);
void threeD_viewport_events(kie_Camera *camera, kie_Scene *scene, renderer_backend *backend, SDL_Window *window, SDL_Event *event, vk_rsrs *rsrs, int selected_object_index);
void threeD_viewport_update(kie_Camera *camera, kie_Scene *scene, renderer_backend *backend, SDL_Window *window, SDL_Event *event, vk_rsrs *rsrs, int selected_object_index);
void threeD_viewport_draw(kie_Camera *camera, kie_Scene *scene, renderer_backend *backend, vk_rsrs *rsrs, int viewport_max_index, bool only_viewport_objects);
void threeD_viewport_render_to_image( kie_Camera *camera, kie_Scene *scene, renderer_backend *backend, SDL_Window *window, SDL_Event *event, vk_rsrs *rsrs, int selected_object_index, char file[KSAI_SMALL_STRING_LENGTH], int camera_id, int pipe_id);
void threeD_viewport_draw_buf(kie_Camera *camera, kie_Scene *scene, renderer_backend *backend, vk_rsrs *rsrs, int viewport_obj_count, bool only_viewport_objects, VkCommandBuffer cmd_buffer);
void threeD_viewport_draw_buf_without_viewport_and_lights(kie_Camera *camera, kie_Scene *scene, renderer_backend *backend, vk_rsrs *rsrs, int viewport_obj_count, VkCommandBuffer cmd_buffer, int pipeline_id);

void threeD_viewport_draw_buf_without_viewport_and_lightsPP(kie_Camera* camera, kie_Scene* scene, renderer_backend* backend, vk_rsrs* rsrs, int viewport_obj_count, VkCommandBuffer cmd_buffer, int pipeline_id);


#define INT_CLR_CNVRT(b, g, r) (ivec3) {roundf(255 * b), roundf(255 * g), roundf(255 * r)}
#define FLT_CLR_CNVRT(b, g, r) (vec3) {((float)b)/255, ((float)g)/255, ((float)r)/255}

__forceinline bool clr_cmp(ivec3 clr1, ivec3 clr2)
{
	return clr1[0] == clr2[0] && clr1[1] == clr2[1] && clr1[2] == clr2[2];
}

__forceinline bool vec3_cmp(vec3 v1, vec3 v2)
{
	return v1[0] == v2[0] && v1[1] == v2[1] && v1[2] == v2[2];
}


