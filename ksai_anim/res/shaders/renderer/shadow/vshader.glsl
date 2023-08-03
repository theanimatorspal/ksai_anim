#version 450
#pragma shader_stage(vertex)
 
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitTangent;
layout(location = 5) in vec2 inTexCoord;

layout(binding = 0) uniform uniform_buffer_object
{
	vec3			v1;
	vec3			v2;
	vec3			v3;

	vec3			light0;
	vec3			light1;
	vec3			light2;
	vec3			light3;
	vec3			light4;
	vec3			light5;
	vec3			light6;
	vec3			light7;
	vec3			light8;

	float			lint0;
	float			lint1;
	float			lint2;
	float			lint3;
	float			lint4;
	float			lint5;
	float			lint6;
	float			lint7;
	float			lint8;

	mat4			model;
	mat4			view;
	mat4			proj;
	vec3			view_dir;
	float			line_thickness;
} ubo;

 
void main()
{
   vec4 pos = vec4(inPosition.x, inPosition.y, inPosition.z, 1.0);
   vec4 world_pos = ubo.model * pos;
   gl_Position = ubo.view * ubo.proj * world_pos;
}


