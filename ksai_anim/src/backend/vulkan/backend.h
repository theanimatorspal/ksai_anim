#pragma once
#include "init.h"
#include <engine/renderer/scene.h>
#include "pipelines.h"

typedef VkDescriptorSet vk_dsset_pair[KSAI_VK_DESCRIPTOR_POOL_SIZE];

typedef struct renderer_backend
{
	pipeline_vk checker_pipeline;
	pipeline_vk ksai_render_pipeline;
	pipeline_vk constant_color;
	pipeline_vk mShadowPipe;

	pipeline_vk mPostProcess;
	VkBuffer mScreenQuadBufferV;
	VkDeviceMemory mScreenQuadVmem;
	VkDeviceMemory mScreenQuadImem;
	VkBuffer mScreenQuadBufferI;

	VkBuffer vbuffer;
	VkDeviceMemory vbuffer_memory;
	VkDeviceSize voffset;

	VkBuffer ibuffer;
	VkDeviceMemory ibuffer_memory;
	VkDeviceSize ioffset;

	VkBuffer ubuffer[MAX_FRAMES_IN_FLIGHT];
	VkDeviceMemory ubuffer_memory[MAX_FRAMES_IN_FLIGHT];
	VkDeviceSize uoffset[MAX_FRAMES_IN_FLIGHT];
	void *udata[MAX_FRAMES_IN_FLIGHT];

	VkDeviceSize *voffsets;
	VkDeviceSize *ioffsets;
	VkDeviceSize(*uoffsets)[2];
	uint32_t offset_count;

	VkDescriptorSet(*descriptor_sets)[MAX_FRAMES_IN_FLIGHT];
	VkDescriptorPool *descriptor_pools;
	VkDescriptorPoolSize pool_sizes[KSAI_VK_DESCRIPTOR_POOL_SIZE];

	VkImage obj_images;
	VkImageView *obj_image_views;
	VkDeviceMemory obj_images_memory;
	uint32_t obj_images_count;

	/* SkyBox */
	pipeline_vk skybox;
	VkImage skybox_image;
	VkImageView skybox_image_view;
	VkDeviceMemory skybox_image_memory;
	VkSampler skybox_sampler;

	/* Mouse Pick OffScreen */
	struct mspk
	{
		VkBuffer bfr_;  /* MOUSEPICK BUFFER*/
		VkDeviceMemory bfr_mmry_;
		void *data_;

		VkBuffer render_buffer;  /* Render BUFFER*/
		VkDeviceMemory render_buffer_memory;
		void *render_buffer_data;

		VkBuffer mShadowBuffer;
		VkDeviceMemory mShadowBufferMemory;
		void *mShadowBufferData;

		VkImage img_clr_att_; /* COLOR ATTACHMENT */
		VkDeviceMemory img_mmry_clr_att_;
		VkImageView img_vw_clr_att_;

		VkImage img_dpth_att_; /* DEPTH ATTACHMENT */
		VkDeviceMemory img_mmry_dpth_att_;
		VkImageView img_vw_dpth_att_;

		VkFramebuffer frm_bfr_;
		VkRenderPass rndr_pss_;
		VkSampler smplr_;
		VkDescriptorImageInfo dscrptr_;
	} mspk;

	struct Shadow
	{
		VkImage mDepthImage;
		VkDeviceMemory mDepthImageMemory;
		VkImageView mView;
		VkRenderPass mRenderPass;
		VkFramebuffer mFrameBuffer;
	} mShadow;

	/* Particle System*/
	struct particle_system
	{
		VkBuffer buffer[MAX_FRAMES_IN_FLIGHT];
		VkDeviceMemory memory[MAX_FRAMES_IN_FLIGHT];
		void *buffer_data[MAX_FRAMES_IN_FLIGHT];

		VkBuffer ubuffer[MAX_FRAMES_IN_FLIGHT];
		VkDeviceMemory ubuffer_memory[MAX_FRAMES_IN_FLIGHT];
		void *ubffer_data[MAX_FRAMES_IN_FLIGHT];

		VkDescriptorSet descriptor_sets[MAX_FRAMES_IN_FLIGHT];
		VkDescriptorPool descriptor_pool[MAX_FRAMES_IN_FLIGHT];

		pipeline_vk particle_pipe;
		struct particle
		{
			alignas(16) vec3 position;
			alignas(16) vec3 velocity;
			alignas(16) vec4 color;
		} particle[MAX_NO_OF_PARTICLES];
	} particle_system;

} renderer_backend;

KSAI_API void copy_scene_to_backend(vk_rsrs *rsrs, kie_Scene *scene, renderer_backend *backend);
KSAI_API void copy_scene_to_backend_reload(vk_rsrs *rsrs, kie_Scene *scene, renderer_backend *backend);

KSAI_API void initialize_backend(vk_rsrs *rsrs, VkInstance *instance);
KSAI_API void destroy_backend(vk_rsrs *_rsrs);

KSAI_API void initialize_renderer_backend(vk_rsrs *rsrs, renderer_backend *backend);
KSAI_API void destroy_renderer_backend(vk_rsrs *rsrs, renderer_backend *backend);


KSAI_API int draw_backend_start(vk_rsrs *_rsrs, renderer_backend *backend);
KSAI_API int draw_backend_begin(vk_rsrs *_rsrs, vec3 color);
KSAI_API void draw_backend_end(vk_rsrs *_rsrs);
KSAI_API void draw_backend_finish(vk_rsrs *_rsrs);
KSAI_API void draw_backend_wait(vk_rsrs *_rsrs);


KSAI_API void end_cmd_buffer_off_dont_use(vk_rsrs *_rsrs);
KSAI_API void begin_cmd_buffer_off_dont_use(vk_rsrs *_rsrs);

KSAI_API int backend_add_texture_to_scene_object(vk_rsrs *rsrs, renderer_backend *backend, kie_Scene *scene, uint32_t obj_index, char *texture_path);


