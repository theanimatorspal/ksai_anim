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

layout(binding = 0) uniform uniform_buffer_object
{
	vec3			v1;
	vec3			v2;
	vec3			v3;
	mat4			model;
	mat4			view;
	mat4			proj;
} ubo;

layout(push_constant, std430) uniform pc {
    mat4 mvp;
    vec3 v1;
} push;


void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    gl_Position.y = - gl_Position.y;
    vert_position = inPosition;
    fragColor = inColor;
    vert_texcoord = inTexCoord;
    vert_color = max(0.0, dot(vert_normal, vec3(0.58, 0.58, 0.58))) + 0.1;


    mat4 model_view = ubo.view * ubo.model;
    vert_normal = mat3( model_view ) * inNormal;
    vert_tangent = mat3( model_view ) * inTangent;
    vert_bitangent = mat3( model_view ) * inBitTangent;

    debugPrintfEXT("mvp(%f, %f, %f):\n", ubo.model[0][0], ubo.model[1][0], ubo.model[2][0]);
    debugPrintfEXT("view(%f, %f, %f):\n", ubo.model[0][0], ubo.model[1][1], ubo.model[2][2]);
    debugPrintfEXT("proj(%f, %f, %f):\n", ubo.model[0][1], ubo.model[1][0], ubo.model[2][1]);
}
