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

void prepare_offscreen(vk_rsrs *_rsrs, renderer_backend *backend);
void destroy_offscreen(vk_rsrs *rsrs, renderer_backend *backend);
void recreate_offscreen(vk_rsrs *_rsrs, renderer_backend *backend);
void render_offscreen_begin(vk_rsrs *rsrs, renderer_backend *backend);
void render_offscreen_end(vk_rsrs *rsrs, renderer_backend *backend);
