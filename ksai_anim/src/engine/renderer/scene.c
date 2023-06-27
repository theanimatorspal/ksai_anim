#include "scene.h"

KSAI_API void kie_Scene_init(kie_Scene *scene)
{
	scene->objects_count = 0;
	scene->objects = (kie_Object*) ksai_Arena_allocate(sizeof(kie_Object) * KSAI_MAX_NO_OF_OBJECTS, &global_object_arena);
}

KSAI_API void kie_Scene_add_object(kie_Scene *scene, kie_Object *obj)
{
	scene->objects[scene->objects_count] = *obj;
	scene->objects_count++;
}
