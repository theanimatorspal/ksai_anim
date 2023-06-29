#pragma once
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
#include <vendor/stbi/stb_image.h>

void prepare_skybox(vk_rsrs *rsrs, renderer_backend *backend);
void destroy_skybox(vk_rsrs *rsrs, renderer_backend *backend);
void draw_skybox_backend(vk_rsrs *rsrs, renderer_backend *backend, kie_Scene *scene);
