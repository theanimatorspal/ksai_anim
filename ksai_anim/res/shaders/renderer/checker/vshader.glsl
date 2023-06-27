#version 450
#pragma shader_stage(vertex)
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;

layout(binding = 0) uniform uniform_buffer_object
{
    vec3 pos;
    vec3 color;
} ubo;

layout(push_constant, std430) uniform pc {
    mat4 mvp;
    vec3 v1;
} push;


void main() {
    gl_Position = push.mvp * vec4(inPosition, 1.0);
    gl_Position.y = - gl_Position.y;
    fragColor = ubo.color;
    fragTexCoord = inTexCoord;
    fragNormal = inNormal;
    debugPrintfEXT("inPos(%f, %f, %f):\n", inPosition[0], inPosition[1], inPosition[2]);
}
