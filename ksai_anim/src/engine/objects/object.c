#include "object.h"
#include <string.h>
ksai_Arena global_object_arena;

void kie_Object_Arena_init()
{
	ksai_Arena_init((size_t) KSAI_OBJECT_ARENA, &global_object_arena);
}

void kie_generate_mvp(mat4 projection, kie_Camera *camera, mat4 model, mat4 mvp)
{
	mat4 temp;
	glm_mat4_mul(projection, camera->view, temp);
	glm_mat4_mul(temp, model, mvp);
}

void kie_Object_init(kie_Object *out_obj)
{
	glm_vec3_copy((vec3) {0, 0, 0}, out_obj->position);
	glm_vec3_copy((vec3) {0, 0, 0}, out_obj->rotation);
	glm_vec3_copy((vec3) {1, 1, 1}, out_obj->scale);
	out_obj->vertices_count = 0;
	out_obj->indices_count = 0;

}

void kie_Object_create_circle(
	kie_Object *out_obj,
	float radius,
	uint32_t divisions,
	vec3 center_point,
	vec3 base_point,
	bool should_allocate,
	uint32_t index_start,
	uint32_t vert_start
)
{
	float r = radius;
	uint32_t cnt = divisions;
	if (should_allocate == true)
	{
		out_obj->vertices_count = cnt + 1;
		out_obj->indices_count = cnt * 3;
		out_obj->vertices = (kie_Vertex*) ksai_Arena_allocate(out_obj->vertices_count * sizeof(kie_Vertex), &global_object_arena);
		out_obj->indices = (uint32_t*) ksai_Arena_allocate(out_obj->indices_count * sizeof(uint32_t), &global_object_arena);
		memset(out_obj->vertices, 0, out_obj->vertices_count * sizeof(kie_Vertex));
		memset(out_obj->indices, 0, out_obj->indices_count * sizeof(uint32_t));
	}
	glm_vec3_copy(center_point, out_obj->vertices[vert_start].position);
	float di = 2 * GLM_PI / cnt;
	int i;
	vec2 texF[3];
	glm_vec2_copy((vec2) {0, 0}, texF[0]);
	glm_vec2_copy((vec2) {1, 0}, texF[1]);
	glm_vec2_copy((vec2) {0.5, 1}, texF[2]);
	for (i = vert_start; i < out_obj->vertices_count - 1; i++)
	{
		float x = r * cos(i * di);
		float z = r * sin(i * di);
		glm_vec3_copy((vec3) { x, base_point[1], z }, out_obj->vertices[i + 1].position);
		glm_vec3_copy(texF[i % 3], out_obj->vertices[i].tex_coord);
	}

	i = vert_start;
	for (int indcs_index = index_start; indcs_index < out_obj->indices_count - 3; indcs_index += 3)
	{
		out_obj->indices[indcs_index] = i + 1;
		out_obj->indices[indcs_index + 1] = vert_start;
		out_obj->indices[indcs_index + 2] = i + 2;
		i++;
	}
	out_obj->indices[out_obj->indices_count - 3] = i + 1;
	out_obj->indices[out_obj->indices_count - 2] = vert_start;
	out_obj->indices[out_obj->indices_count - 1] = vert_start + 1;
}

void kie_Object_Arena_destroy()
{
	ksai_Area_free(&global_object_arena);
}
