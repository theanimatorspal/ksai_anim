#version 450
#pragma shader_stage(vertex)
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitTangent;
layout(location = 5) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 vert_texcoord;
layout(location = 2) out vec3 vert_normal;
layout(location = 3) out float vert_color;
layout(location = 4) out vec3 vert_position;
layout(location = 5) out vec3 vert_tangent;
layout(location = 6) out vec3 vert_bitangent;
layout(location = 7) out vec3 vert_texcoord3;

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
} ubo;


layout(push_constant, std430) uniform pc {
    mat4 mvp;
    vec3 v1;
} push;


void main() {
    vec3 position = mat3(ubo.proj * ubo.view * ubo.model) * inPosition.xyz;
    gl_Position  = (ubo.proj *vec4(position, 0.0)).xyzz;
    fragColor = inColor;
    vert_texcoord3 = inPosition.xyz;
}
