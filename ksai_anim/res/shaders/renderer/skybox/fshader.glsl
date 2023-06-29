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
	mat4			model;
	mat4			view;
	mat4			proj;
} ubo;


layout(binding = 1) uniform sampler2D texSampler;


void main()
{

	/* Diffuse */
	vec3 light_pos = vec3(5, 5, 5);

	float diffuse = vert_color;
	vec3 normal_vector = normalize(vert_normal);
	vec3 light_dir = normalize(light_pos - vert_position);
	diffuse = max(0.0, dot(normal_vector, light_dir)) + 0.1;
	out_color = texture(texSampler, vert_texcoord) * diffuse;

	/* Normal Maps */
	/*
	vec3 normal = 2 * texture(texSampler, vert_texcoord).rgb - 1.0;
	vec3 normal_vector = normalize(mat3(vert_tangent, vert_bitangent, vert_normal) * normal);
	vec3 light_vector = normalize(light_pos - vert_position);
	float diffuse_term = max(0.0, dot(normal_vector, light_vector)) * max(0.0, dot(vert_normal, light_vector));
	out_color = vec4(diffuse_term + 0.1);
	*/

	if(diffuse > 0.0)
	{
		vec3 half_vector = normalize(normalize(-vert_position.xyz) + light_dir);
		float specular_term = pow(dot(half_vector, normal_vector), 60.0);
		out_color += vec4(specular_term);
	}



	out_color.a = 1;
}
