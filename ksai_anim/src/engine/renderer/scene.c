#include "scene.h"
#include <stdarg.h>

KSAI_API void kie_Scene_init(kie_Scene *scene)
{
	scene->objects_count = 0;
	scene->lights_count = 0;
	scene->objects = (kie_Object *) ksai_Arena_allocate(sizeof(kie_Object) * KSAI_MAX_NO_OF_OBJECTS, &global_object_arena);
}

KSAI_API void kie_Scene_add_object(kie_Scene *scene, int args, ...)
{
	va_list object_list;
	va_start(object_list, args);

	for (int i = 0; i < args; i++)
	{
		scene->objects[scene->objects_count] = *va_arg(object_list, kie_Object*);
		scene->objects_count++;
	}

	va_end(object_list);
}
