#pragma once
#include <vendor/cglm/cglm.h>

typedef struct kie_Frame
{
	vec3 position;
	vec3 rotation;
	vec3 scale;
	vec3 color;


	vec3 direction;
	float intensity;
	float area;

    struct camera {
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
    } camera;

    uint32_t frame_time;
    enum type {
        LINEAR,
        EXPONENTIAL,
        CONSTANT,
        QUADRATIC,
        CUBIC
    } type;
} kie_Frame;

