#version 450
#pragma shader_stage(fragment)

layout(push_constant, std430) uniform pc {
    mat4 mvp;
    vec3 vt;
} push;

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform uniform_buffer_object
{
    vec3 color;
    vec3 color1;
    vec3 color2;
} ubo;

void main()
{
	out_color =  vec4(push.vt, 1);
}
