#version 450
#pragma shader_stage(fragment)
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 vert_texcoord;
layout(location = 2) in vec3 vert_normal;
layout(location = 3) in float vert_color;
layout(location = 4) in vec3 vert_position;
layout(location = 5) in vec3 vert_tangent;
layout(location = 6) in vec3 vert_bitangent;

layout(location = 0) out vec4 out_color;


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


layout(binding = 1) uniform sampler2D texSampler;


void main()
{
	out_color = (texture(texSampler, vert_texcoord));
}
