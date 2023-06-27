#version 450
#pragma shader_stage(fragment)

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;

layout(location = 0) out vec4 out_color;
layout(binding = 1) uniform sampler2D texSampler;


void main()
{
	out_color = texture(texSampler, fragTexCoord);
	out_color.a = 1;
}
