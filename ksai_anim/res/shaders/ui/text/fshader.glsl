#version 450
#pragma shader_stage(fragment)

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 out_color;

layout(binding = 1) uniform sampler2D texSampler;

layout(push_constant, std430) uniform pc {
    mat4 mvp;
    vec3 vt;
} push;


const float smoothing = 1.0/16.0;

void main()
{
    float distance = texture(texSampler, fragTexCoord).a;
    float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);
    vec4 color = texture(texSampler, fragTexCoord);
    out_color = vec4(color.xyz, color.w * alpha);
    out_color.x = push.vt.x;
    out_color.y = push.vt.y;
    out_color.z = push.vt.z;
}
