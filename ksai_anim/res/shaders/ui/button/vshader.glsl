#version 450
#pragma shader_stage(vertex)
#extension GL_EXT_debug_printf : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;


layout(push_constant, std430) uniform pc {
    mat4 mvp;
    vec3 vt;
} push;


void main() {
    gl_Position = push.mvp * vec4(inPosition, 1.0);
//  gl_Position.y = - gl_Position.y;
    fragColor = vec3(1, 1, 1);
    fragTexCoord = inTexCoord;
//  debugPrintfEXT("Pos(%f, %f, %f, %f):\n", gl_Position[0], gl_Position[1], gl_Position[2], gl_Position[3]);
    debugPrintfEXT("inPos(%f, %f, %f):\n", inPosition[0], inPosition[1], inPosition[2]);
}
