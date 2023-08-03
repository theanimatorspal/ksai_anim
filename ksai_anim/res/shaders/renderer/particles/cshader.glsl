#version 450
#pragma shader_stage(compute)

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

struct Particle {
    vec3 position;
    vec3 velocity;
    vec4 color;
};

layout(std140, binding = 1) readonly buffer ParticleSSBOIn {
   Particle particlesIn[ ];
};

layout(std140, binding = 2) buffer ParticleSSBOOut {
   Particle particlesOut[ ];
};

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

void main() 
{
    uint index = gl_GlobalInvocationID.x;  

    Particle particleIn = particlesIn[index];

    particlesOut[index].position = vec3(particleIn.position.xy + particleIn.velocity.xy * ubo.v1.x, 1); // time delta
    particlesOut[index].velocity = particleIn.velocity;
}