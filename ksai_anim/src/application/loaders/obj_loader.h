#pragma once
#include <engine/objects/object.h>

void read_obj_to_kie_Object(const char *path, kie_Object *out_obj);

void read_add_auto_objs(kie_Scene *scene, const char path[KSAI_SMALL_STRING_LENGTH]);
