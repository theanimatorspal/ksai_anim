#pragma once
#include <engine/objects/object.h>

typedef struct kie_Scene
{
	kie_Object *objects;
	uint32_t objects_count;
} kie_Scene;

KSAI_API void kie_Scene_init(kie_Scene *scene);
KSAI_API void kie_Scene_add_object(kie_Scene *scene, kie_Object *obj);
