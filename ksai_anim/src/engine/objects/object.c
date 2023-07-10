#include "object.h"
#include <string.h>
ksai_Arena global_object_arena;

void kie_Object_Arena_init()
{
	ksai_Arena_init((size_t)KSAI_OBJECT_ARENA, &global_object_arena);
}

void kie_generate_mvp(mat4 projection, kie_Camera *camera, mat4 model, mat4 mvp)
{
	mat4 temp;
	glm_mat4_mul(projection, camera->view, temp);
	glm_mat4_mul(temp, model, mvp);
}

void kie_Object_init(kie_Object *out_obj)
{
	glm_vec3_copy((vec3){0, 0, 0}, out_obj->position);
	glm_vec3_copy((vec3){0, 0, 0}, out_obj->rotation);
	glm_vec3_copy((vec3){0, 0, 0}, out_obj->color);
	glm_vec3_copy((vec3){1, 1, 1}, out_obj->scale);
	out_obj->vertices_count = 0;
	out_obj->indices_count = 0;
	out_obj->is_light = false;
	out_obj->is_camera = false;
	out_obj->frames = (kie_Frame *)ksai_Arena_allocate(sizeof(kie_Frame) * KSAI_MAX_NO_OF_KEYFRAMES, &global_object_arena);
	out_obj->curr_frame = 0;
}

void kie_Camera_init(kie_Camera *out_cam)
{
	out_cam->direction[0] = 0.0f;
	out_cam->direction[1] = 0.0f;
	out_cam->direction[2] = 0.0f;
	out_cam->target[0] = 0.0f;
	out_cam->target[1] = 0.0f;
	out_cam->target[2] = 0.0f;
	out_cam->up[0] = 0.0f;
	out_cam->up[1] = 1.0f;
	out_cam->up[2] = 0.0f;
	out_cam->position[0] = 0.0f;
	out_cam->position[1] = 0.0f;
	out_cam->position[2] = 0.0f;
	out_cam->rotation[0] = 0.0f;
	out_cam->rotation[1] = 0.0f;
	out_cam->rotation[2] = 0.0f;
	glm_vec3_copy((vec3){0, 0, 0}, out_cam->pivot);
	out_cam->fov = 0.7863;
	out_cam->w = 1920;
	out_cam->h = 1080;
}

void kie_Object_copy(kie_Object *out_obj, kie_Object *in_obj)
{
	glm_vec3_copy(in_obj->position, out_obj->position);
	glm_vec3_copy(in_obj->rotation, out_obj->rotation);
	glm_vec3_copy(in_obj->color, out_obj->color);
	glm_vec3_copy(in_obj->scale, out_obj->scale);
	out_obj->vertices_count = in_obj->vertices_count;
	out_obj->indices_count = in_obj->indices_count;
	out_obj->vertices = in_obj->vertices;
	out_obj->indices = in_obj->indices;
}

void kie_Object_create_circle(
	kie_Object *out_obj,
	float radius,
	uint32_t divisions,
	vec3 center_point,
	vec3 base_point,
	bool should_allocate,
	uint32_t index_start,
	uint32_t vert_start)
{
	float r = radius;
	uint32_t cnt = divisions;
	if (should_allocate == true)
	{
		out_obj->vertices_count = cnt + 1;
		out_obj->indices_count = cnt * 3;
		out_obj->vertices = (kie_Vertex *)ksai_Arena_allocate(out_obj->vertices_count * sizeof(kie_Vertex), &global_object_arena);
		out_obj->indices = (uint32_t *)ksai_Arena_allocate(out_obj->indices_count * sizeof(uint32_t), &global_object_arena);
		memset(out_obj->vertices, 0, out_obj->vertices_count * sizeof(kie_Vertex));
		memset(out_obj->indices, 0, out_obj->indices_count * sizeof(uint32_t));
	}
	glm_vec3_copy(center_point, out_obj->vertices[vert_start].position);
	float di = 2 * GLM_PI / cnt;
	int i;
	vec2 texF[3];
	glm_vec2_copy((vec2){0, 0}, texF[0]);
	glm_vec2_copy((vec2){1, 0}, texF[1]);
	glm_vec2_copy((vec2){0.5, 1}, texF[2]);
	for (i = vert_start; i < out_obj->vertices_count - 1; i++)
	{
		float x = r * cos(i * di);
		float z = r * sin(i * di);
		glm_vec3_copy((vec3){x, base_point[1], z}, out_obj->vertices[i + 1].position);
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

void kie_Object_create_cylinder(kie_Object *out_obj, float _radius, uint32_t _p_cnt, float _h)
{
	// float r = _radius;
	uint32_t cnt = _p_cnt;
	// int tot_indcs_before_middle_faces = 2 * cnt * 3;
	out_obj->vertices_count = 2 * (cnt + 1);
	out_obj->indices_count = 2 * cnt * 3 + 6 * cnt;
	out_obj->vertices = (kie_Vertex *)ksai_Arena_allocate(out_obj->vertices_count * sizeof(kie_Vertex), &global_object_arena);
	out_obj->indices = (uint32_t *)ksai_Arena_allocate(out_obj->indices_count * sizeof(uint32_t), &global_object_arena);
	memset(out_obj->vertices, 0, out_obj->vertices_count * sizeof(kie_Vertex));
	memset(out_obj->indices, 0, out_obj->indices_count * sizeof(uint32_t));

	out_obj->vertices_count = cnt + 1;
	out_obj->indices_count = cnt * 3;
	int lower_vstart = 0;
	kie_Object_create_circle(out_obj, _radius, _p_cnt, (vec3){0, 0, 0}, (vec3){0, 0, 0}, false, 0, 0);

	out_obj->vertices_count = 2 * (cnt + 1);
	out_obj->indices_count = 2 * cnt * 3;
	int upper_vstart = (cnt + 1);
	kie_Object_create_circle(out_obj, _radius, _p_cnt, (vec3){0, _h, 0}, (vec3){0, _h, 0}, false, cnt * 3, cnt + 1);

	out_obj->indices_count = 2 * cnt * 3 + 6 * cnt;

	int v1_index = lower_vstart + 1;
	int v2_index = lower_vstart + 2;
	int v3_index = upper_vstart + 1;
	int v4_index = upper_vstart + 2;
	int i = 0;
	for (i = 2 * cnt * 3; i < out_obj->indices_count - 6; i += 6)
	{
		out_obj->indices[i] = v1_index;
		out_obj->indices[i + 1] = v2_index;
		out_obj->indices[i + 2] = v3_index;
		out_obj->indices[i + 3] = v3_index;
		out_obj->indices[i + 4] = v4_index;
		out_obj->indices[i + 5] = v2_index;

		v1_index++;
		v2_index++;
		v3_index++;
		v4_index++;
	}

	out_obj->indices[i] = v1_index;
	out_obj->indices[i + 1] = lower_vstart + 1;
	out_obj->indices[i + 2] = v3_index;
	out_obj->indices[i + 3] = v3_index;
	out_obj->indices[i + 4] = upper_vstart + 1;
	out_obj->indices[i + 5] = lower_vstart + 1;
}

void kie_Object_join(kie_Object *mesh1, kie_Object *mesh2)
{
	kie_Vertex *vertices = (kie_Vertex *)ksai_Arena_allocate(((size_t)mesh1->vertices_count + mesh2->vertices_count) * sizeof(kie_Vertex), &global_object_arena);
	uint32_t *indices = (uint32_t *)ksai_Arena_allocate(((size_t)mesh1->indices_count + mesh2->indices_count) * sizeof(uint32_t), &global_object_arena);
	memcpy(vertices, mesh1->vertices, mesh1->vertices_count * sizeof(kie_Vertex));
	memcpy(indices, mesh1->indices, mesh1->indices_count * sizeof(uint32_t));

	memcpy(vertices + mesh1->vertices_count, mesh2->vertices, mesh2->vertices_count * sizeof(kie_Vertex));

	int j = 0;
	for (int i = mesh1->indices_count; i < mesh1->indices_count + mesh2->indices_count; i++)
	{
		indices[mesh1->indices_count + j] = mesh2->indices[j] + mesh1->vertices_count;
		j++;
	}

	mesh1->vertices = vertices;
	mesh1->indices = indices;
	mesh1->indices_count += mesh2->indices_count;
	mesh1->vertices_count += mesh2->vertices_count;
}

void kie_Object_Arena_destroy()
{
	ksai_Area_free(&global_object_arena);
}

static int compare_kie_Frames(const void *a, const void *b)
{
	kie_Frame *frame1 = (kie_Frame *)a;
	kie_Frame *frame2 = (kie_Frame *)b;
	if (frame1->frame_time > frame2->frame_time)
		return 1;
	else
		return 0;
}

static void kie_Frames_sort(kie_Object *object)
{
	// qsort_s(object->frames, object->curr_frame, sizeof(kie_Frame), compare_kie_Frames, NULL);
	qsort(object->frames, object->curr_frame, sizeof(kie_Frame), compare_kie_Frames);
}

void kie_Frame_set(kie_Object *object, uint32_t frame_time)
{
	kie_Frame frame;
	frame.frame_time = frame_time;
	glm_vec3_copy(object->position, frame.position);
	glm_vec3_copy(object->rotation, frame.rotation);
	glm_vec3_copy(object->scale, frame.scale);
	glm_vec3_copy(object->color, frame.color);

	glm_vec3_copy(object->direction, frame.direction);
	frame.intensity = object->intensity;
	frame.area = object->area;

	glm_vec3_copy(object->camera.position, frame.camera.position);
	glm_vec3_copy(object->camera.rotation, frame.camera.position);
	glm_vec3_copy(object->camera.target, frame.camera.position);
	glm_vec3_copy(object->camera.direction, frame.camera.position);
	glm_vec3_copy(object->camera.up, frame.camera.position);
	glm_vec3_copy(object->camera.right, frame.camera.position);
	glm_vec3_copy(object->camera.pivot, frame.camera.position);
	glm_mat4_copy(object->camera.view, frame.camera.view);

	frame.camera.fov = object->camera.fov;
	frame.camera.w = object->camera.w;
	frame.camera.h = object->camera.h;
	frame.type = LINEAR;

	object->frames[object->curr_frame] = frame;
	object->curr_frame++;
	kie_Frames_sort(object);
}

void kie_Frame_delete(kie_Object *object, uint32_t frame_time)
{
	for (int i = 0; i < object->curr_frame; i++)
	{
		if (object->frames[i].frame_time == frame_time)
		{
			memmove_s(object->frames + i, (size_t)(object->curr_frame - i - 1) * sizeof(kie_Frame), object->frames + i + 1, sizeof(kie_Frame) * (size_t)(object->curr_frame - i - 1));
		}
	}
	object->curr_frame--;
}

static void copy_frame_to_object(kie_Frame *frame, kie_Object *object)
{
	glm_vec3_copy(frame->position, object->position);
	glm_vec3_copy(frame->rotation, object->rotation);
	glm_vec3_copy(frame->scale, object->scale);
	glm_vec3_copy(frame->color, object->color);
	glm_vec3_copy(frame->direction, object->direction);
	object->intensity = frame->intensity;
	object->area = frame->area;
	glm_vec3_copy(frame->camera.position, object->camera.position);
	glm_vec3_copy(frame->camera.rotation, object->camera.rotation);
	glm_vec3_copy(frame->camera.target, object->camera.target);
	glm_vec3_copy(frame->camera.direction, object->camera.direction);
	glm_vec3_copy(frame->camera.up, object->camera.up);
	glm_vec3_copy(frame->camera.right, object->camera.right);
	glm_vec3_copy(frame->camera.pivot, object->camera.pivot);
	glm_mat4_copy(frame->camera.view, object->camera.view);
	object->camera.fov = frame->camera.fov;
	object->camera.w = frame->camera.w;
	object->camera.h = frame->camera.h;
}

static void kie_Frame_interp(kie_Object *object, int i, int j, int frame_time)
{
	kie_Frame frame1 = object->frames[i];
	kie_Frame frame2 = object->frames[j];
	if (frame1.type == frame2.type)
	{
		float bias = glm_percentc(frame1.frame_time, frame2.frame_time, frame_time);
		glm_vec3_copy((vec3){glm_lerp(frame1.position[0], frame2.position[0], bias), glm_lerp(frame1.position[1], frame2.position[1], bias), glm_lerp(frame1.position[2], frame2.position[2], bias)}, object->position);
		glm_vec3_copy((vec3){glm_lerp(frame1.rotation[0], frame2.rotation[0], bias), glm_lerp(frame1.rotation[1], frame2.rotation[1], bias), glm_lerp(frame1.rotation[2], frame2.rotation[2], bias)}, object->rotation);
		glm_vec3_copy((vec3){glm_lerp(frame1.scale[0], frame2.scale[0], bias), glm_lerp(frame1.scale[1], frame2.scale[1], bias), glm_lerp(frame1.scale[2], frame2.scale[2], bias)}, object->scale);
		glm_vec3_copy((vec3){glm_lerp(frame1.color[0], frame2.color[0], bias), glm_lerp(frame1.color[1], frame2.color[1], bias), glm_lerp(frame1.color[2], frame2.color[2], bias)}, object->color);
		object->intensity = glm_lerp(frame1.intensity, frame2.intensity, bias);
		object->area = glm_lerp(frame1.area, frame2.area, object->area);
		glm_vec3_copy((vec3){glm_lerp(frame1.camera.position[0], frame2.camera.position[0], bias), glm_lerp(frame1.camera.position[1], frame2.camera.position[1], bias), glm_lerp(frame1.camera.position[2], frame2.camera.position[2], bias)}, object->camera.position);
		glm_vec3_copy((vec3){glm_lerp(frame1.camera.rotation[0], frame2.camera.rotation[0], bias), glm_lerp(frame1.camera.rotation[1], frame2.camera.rotation[1], bias), glm_lerp(frame1.camera.rotation[2], frame2.camera.rotation[2], bias)}, object->camera.rotation);
		glm_vec3_copy((vec3){glm_lerp(frame1.camera.target[0], frame2.camera.target[0], bias), glm_lerp(frame1.camera.target[1], frame2.camera.target[1], bias), glm_lerp(frame1.camera.target[2], frame2.camera.target[2], bias)}, object->camera.target);
		glm_vec3_copy((vec3){glm_lerp(frame1.camera.direction[0], frame2.camera.direction[0], bias), glm_lerp(frame1.camera.direction[1], frame2.camera.direction[1], bias), glm_lerp(frame1.camera.direction[2], frame2.camera.direction[2], bias)}, object->camera.direction);
		glm_vec3_copy((vec3){glm_lerp(frame1.camera.up[0], frame2.camera.up[0], bias), glm_lerp(frame1.camera.up[1], frame2.camera.up[1], bias), glm_lerp(frame1.camera.up[2], frame2.camera.up[2], bias)}, object->camera.up);
		glm_vec3_copy((vec3){glm_lerp(frame1.camera.right[0], frame2.camera.right[0], bias), glm_lerp(frame1.camera.right[1], frame2.camera.right[1], bias), glm_lerp(frame1.camera.right[2], frame2.camera.right[2], bias)}, object->camera.right);
		glm_vec3_copy((vec3){glm_lerp(frame1.camera.pivot[0], frame2.camera.pivot[0], bias), glm_lerp(frame1.camera.pivot[1], frame2.camera.pivot[1], bias), glm_lerp(frame1.camera.pivot[2], frame2.camera.pivot[2], bias)}, object->camera.pivot);
		// glm_vec3_copy( (vec3) { glm_lerp(frame1.camera.view[0], frame2.camera.view[0], bias), glm_lerp(frame1.camera.view[1], frame2.camera.view[1], bias), glm_lerp(frame1.camera.view[2], frame2.camera.view[2], bias) }, object->camera.view);
		object->camera.fov = glm_lerp(frame1.camera.fov, frame2.camera.fov, object->camera.fov);
		object->camera.w = glm_lerp(frame1.camera.w, frame2.camera.w, object->camera.w);
		object->camera.h = glm_lerp(frame1.camera.h, frame2.camera.h, object->camera.h);
	}
}

void kie_Frame_eval(kie_Object *object, uint32_t frame_time)
{
	/* If Current Frame time contains a keyframe */
	for (int i = 0; i < object->curr_frame; i++)
	{
		if (object->frames[i].frame_time == frame_time)
		{
			copy_frame_to_object(&object->frames[i], object);
			return;
		}
	}

	/* If Current Frame time is between two keyframes */
	if (object->curr_frame > 0)
	{
		for (int i = 0; i < object->curr_frame - 1;)
		{
			int j = i + 1;
			{
				if (frame_time > object->frames[i].frame_time && frame_time < object->frames[j].frame_time)
				{
					kie_Frame_interp(object, i, j, frame_time);
					return;
				}
			}
			i++;
			j++;
		}
	}
}

bool kie_Frame_has(kie_Object *object, uint32_t frame_time)
{
	for (int i = 0; i < object->curr_frame; i++)
	{
		if (object->frames[i].frame_time == frame_time)
		{
			return true;
		}
	}
}
