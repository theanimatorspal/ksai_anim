#pragma once
#include "init.h"

void initialize_backend(vk_rsrs *rsrs, VkInstance *instance);


int draw_backend_start(vk_rsrs *_rsrs);
int draw_backend_begin(vk_rsrs *_rsrs);
void draw_backend_end(vk_rsrs *_rsrs);
void draw_backend_finish(vk_rsrs *_rsrs);
void draw_backend_wait(vk_rsrs *_rsrs);
