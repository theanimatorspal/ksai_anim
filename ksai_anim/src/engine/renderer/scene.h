#pragma once
#include <engine/objects/object.h>

typedef struct kie_Scene
{
	kie_Object *objects;
	uint32_t objects_count;
	uint32_t lights_count;
} kie_Scene;

KSAI_API void kie_Scene_init(kie_Scene *scene);
KSAI_API void kie_Scene_add_object(kie_Scene *scene, int args, ...);
KSAI_INLINE void kie_Object_add_light_object(kie_Object *out_obj, kie_Scene *scene)
{
	out_obj->is_light = true;
	scene->lights_count++;
	kie_Scene_add_object(scene, 1, out_obj);
}
