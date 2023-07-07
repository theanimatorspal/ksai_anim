#pragma once
#include <ksai/ksai.h>
#include <stdalign.h>
#include <ksai/ksai_memory.h>

typedef struct kie_Camera
{
	vec3 position;
	vec3 rotation; 
	vec3 target; 
	vec3 direction;
	vec3 up;
	vec3 right;
	vec3 pivot;
	mat4 view;
	float fov;
	float w;
	float h;
} kie_Camera;

extern ksai_Arena global_object_arena;

typedef struct kie_Vertex
{
	alignas(16) vec3 position;
	alignas(16) vec3 normal;
	alignas(16) vec3 color;
	alignas(16) vec3 tangent;
	alignas(16) vec3 bit_tangent;
	alignas(8) vec2 tex_coord;
} kie_Vertex;


typedef struct kie_Object
{
	kie_Vertex *vertices;
	uint32_t *indices;

	uint32_t vertices_count;
	uint32_t indices_count;
	vec3 position;
	vec3 rotation;
	vec3 scale;
	vec3 color;

	bool has_texture;
	uint32_t texture_id;

	bool is_light;
	vec3 direction;
	float intensity;
	float area;

	bool is_camera;
	kie_Camera camera;
} kie_Object;


void kie_Object_Arena_init();

void kie_generate_mvp(mat4 projection, kie_Camera *camera, mat4 model, mat4 mvp);

void kie_Object_init(kie_Object *out_obj);

void kie_Camera_init(kie_Camera *out_cam);

void kie_Object_copy(kie_Object *out_obj, kie_Object *in_obj);

void kie_Object_create_circle(kie_Object *out_obj, float radius, uint32_t divisions, vec3 center_point, vec3 base_point, bool should_allocate, uint32_t index_start, uint32_t vert_start);

void kie_Object_create_cylinder(kie_Object *out_obj, float _radius, uint32_t _p_cnt, float _h);

void kie_Object_join(kie_Object *mesh1, kie_Object *mesh2);

void kie_Object_Arena_destroy();

