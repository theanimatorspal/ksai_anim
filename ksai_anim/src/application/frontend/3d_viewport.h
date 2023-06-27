#pragma once
#include <engine/objects/object.h>
#include <SDL2/SDL.h>


void threeD_viewport_init(kie_Camera *camera);
void threeD_viewport_events(kie_Camera *camera, SDL_Window *window, SDL_Event *event);
void threeD_viewport_draw(kie_Camera *camera, kie_Scene *scene, renderer_backend *backend, vk_rsrs *rsrs);
