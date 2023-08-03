extern "C"
{
#include <backend/vulkan/backend.h>
#include "3d_viewport.h"
}


#include "vulkan/vulkan.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "backend/vulkan/VulkanPipelines.hpp"

static auto ComputeViewMatrixForRotation(glm::vec3 inOrigin, glm::vec3 inRot) -> glm::mat4
{
	glm::mat4 mat(1.0);
	float rx = glm::degrees(inRot.x);
	float ry = glm::degrees(inRot.y);
	float rz = glm::degrees(inRot.z);
	mat = glm::rotate(mat, -rx, glm::vec3(1, 0, 0));
	mat = glm::rotate(mat, -ry, glm::vec3(0, 1, 0));
	mat = glm::rotate(mat, -rz, glm::vec3(0, 0, 1));
	mat = glm::translate(mat, -inOrigin);
	return mat;
}

static auto CreateShadowMapRenderPass(const vk::Device& dev) -> vk::RenderPass
{
	std::array<vk::AttachmentDescription, 1> Attachments;
	Attachments[0] =
		vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), vk::Format::eD32Sfloat)
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eStore)
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

	vk::AttachmentReference DepthRef(0, vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::SubpassDescription Subpasses = vk::SubpassDescription(vk::SubpassDescriptionFlags())
		.setPDepthStencilAttachment(&DepthRef);

	vk::RenderPassCreateInfo RenderPassCreateInfo = vk::RenderPassCreateInfo(
		vk::RenderPassCreateFlags(),
		0,
		nullptr,
		1
	).setSubpasses(Subpasses)
		.setAttachmentCount(Attachments.size())
		.setAttachments(Attachments)
		.setDependencyCount(0);

	return dev.createRenderPass(RenderPassCreateInfo);
}

extern "C" {
	void PrepareForShadows(const vk_rsrs* rsrs, renderer_backend* inBackend)
	{
		auto Device = vk::Device(vk_logical_device_);
		vk::ImageCreateInfo ImageCreateInfo = vk::ImageCreateInfo(
			vk::ImageCreateFlags(),
			vk::ImageType::e2D,
			vk::Format::eD32Sfloat,
			vk::Extent3D(rsrs->vk_swap_chain_image_extent_2d_, 1),
			1,
			1,
			vk::SampleCountFlagBits::e1,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled,
			vk::SharingMode::eExclusive,
			0,
			nullptr,
			vk::ImageLayout::eUndefined,
			nullptr
		);

		inBackend->mShadow.mDepthImage = Device.createImage(ImageCreateInfo);

		auto ImageViewCreateInfo = vk::ImageViewCreateInfo(
			vk::ImageViewCreateFlags(),
			inBackend->mShadow.mDepthImage,
			vk::ImageViewType::e2D,
			vk::Format::eD32Sfloat,
			vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB),
			vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1)
		);

		vk::ImageMemoryRequirementsInfo2 info(inBackend->mShadow.mDepthImage);
		auto MemoryRequirements = Device.getImageMemoryRequirements2(info);
		auto MemoryAllocateInfo = vk::MemoryAllocateInfo(MemoryRequirements.memoryRequirements.size)
			.setMemoryTypeIndex(find_memory_type_util(MemoryRequirements.memoryRequirements.memoryTypeBits,
				static_cast<VkMemoryPropertyFlags>(vk::MemoryPropertyFlagBits::eDeviceLocal)));
		inBackend->mShadow.mDepthImageMemory = Device.allocateMemory(MemoryAllocateInfo);


		Device.bindImageMemory(inBackend->mShadow.mDepthImage, inBackend->mShadow.mDepthImageMemory, 0);
		inBackend->mShadow.mView = Device.createImageView(ImageViewCreateInfo);

		glm::mat4 clip = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.0f, 0.0f, 0.5f, 1.0f);

		glm::mat4 light_projection = clip *
			glm::perspective(glm::radians(45.0f),
				static_cast<float>(rsrs->vk_swap_chain_image_extent_2d_.width)
				/ static_cast<float>(rsrs->vk_swap_chain_image_extent_2d_.height),
				0.1f,
				100.0f);
		inBackend->mShadow.mRenderPass = CreateShadowMapRenderPass(Device);

		std::array<vk::ImageView, 1> ShadowAttachment = { inBackend->mShadow.mView };
		auto FrameBufferCreateInfo = vk::FramebufferCreateInfo()
			.setRenderPass(inBackend->mShadow.mRenderPass)
			.setAttachmentCount(1)
			.setAttachments(ShadowAttachment)
			.setWidth(rsrs->vk_swap_chain_image_extent_2d_.width)
			.setHeight(rsrs->vk_swap_chain_image_extent_2d_.height)
			.setLayers(1);

		inBackend->mShadow.mFrameBuffer = Device.createFramebuffer(FrameBufferCreateInfo);

		VulkanPipeline::createPipelineForShadow(rsrs, inBackend, &inBackend->mShadowPipe, inBackend->mShadow.mRenderPass);
	}

	void DestroyForShadows(vk_rsrs* rsrs, renderer_backend* backend)
	{
		auto Device = vk::Device(vk_logical_device_);
		Device.destroyImage(backend->mShadow.mDepthImage);
		Device.freeMemory(backend->mShadow.mDepthImageMemory);
		Device.destroyFramebuffer(backend->mShadow.mFrameBuffer);
		Device.destroyImageView(backend->mShadow.mView);
		Device.destroyRenderPass(backend->mShadow.mRenderPass);
		pipeline_vk_destroy3(&backend->mShadowPipe);
	}

	void DrawShadows(kie_Camera* camera, kie_Scene* scene, uint32_t viewport_obj_count, vk_rsrs* rsrs, renderer_backend* backend, VkCommandBuffer inCmdBuffer)
	{
		vk::Device Device(vk_logical_device_);
		auto ClearValue = vk::ClearValue()
			.setDepthStencil(vk::ClearDepthStencilValue(1.0f, 0.0f));
		auto BeginInfo = vk::RenderPassBeginInfo(
			backend->mShadow.mRenderPass,
			backend->mShadow.mFrameBuffer,
			vk::Rect2D(vk::Offset2D(0.0f, 0.0f),
				rsrs->vk_swap_chain_image_extent_2d_),
			ClearValue);
		vk::CommandBuffer CmdBuffer(inCmdBuffer);
		//CmdBuffer.begin(vk::CommandBufferBeginInfo());
		CmdBuffer.beginRenderPass(BeginInfo, vk::SubpassContents::eInline);

		auto Viewport = vk::Viewport(
			0.0f,
			0.0f,
			rsrs->vk_swap_chain_image_extent_2d_.width,
			rsrs->vk_swap_chain_image_extent_2d_.height,
			0.0f, 1.0f);
		CmdBuffer.setViewport(0, Viewport);

		auto Scissor = vk::Rect2D(
			vk::Offset2D(0, 0),
			rsrs->vk_swap_chain_image_extent_2d_);
		CmdBuffer.setScissor(0, Scissor);
		kie_Camera Ncamera = *camera;
		glm_vec3_copy((vec3) { -5.0f, 5.0f, 5.0f }, Ncamera.position);
		threeD_viewport_draw_buf_without_viewport_and_lightsPP(
			&Ncamera,
			scene,
			backend,
			rsrs,
			viewport_obj_count,
			CmdBuffer,
			2
		);
		CmdBuffer.endRenderPass();
	}

	void PrepareForPostProcessing(const vk_rsrs* rsrs, renderer_backend* inBackend)
	{
		std::array<kie_Vertex, 4> vertices = { };
		std::array<uint32_t, 6> indices = { };

		vk::Device Device(vk_logical_device_);
		VkDeviceSize vsize = vertices.size() * sizeof(kie_Vertex);
		create_buffer_util(
			vsize,
			static_cast<VkBufferUsageFlags>(vk::BufferUsageFlagBits::eVertexBuffer),
			static_cast<VkMemoryPropertyFlags>(vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible),
			&inBackend->mScreenQuadBufferV,
			&inBackend->mScreenQuadVmem,
			Device
		);
		void* vdata;
		void* stuff = Device.mapMemory(inBackend->mScreenQuadVmem, 0, vsize);
		if (!stuff)
		{
			std::memcpy(vdata, vertices.data(), vsize);
			Device.unmapMemory(inBackend->mScreenQuadVmem);
		}

		vsize = indices.size() * sizeof(uint32_t);
		create_buffer_util(
			vsize,
			static_cast<VkBufferUsageFlags>(vk::BufferUsageFlagBits::eIndexBuffer),
			static_cast<VkMemoryPropertyFlags>(vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible),
			&inBackend->mScreenQuadBufferI,
			&inBackend->mScreenQuadImem,
			Device
		);
		void* idata;
		void* stuffi = Device.mapMemory(inBackend->mScreenQuadImem, 0, vsize);
		if (!stuffi)
		{
			std::memcpy(idata, indices.data(), vsize);
			Device.unmapMemory(inBackend->mScreenQuadImem);
		}


		VulkanPipeline::createPipelineForPP(rsrs, inBackend, &inBackend->mPostProcess, rsrs->vk_render_pass_);
	}

	void DestroyForPostProcessing(const vk_rsrs* rsr, renderer_backend* backend)
	{
		pipeline_vk_destroy3(&backend->mPostProcess);
	}
}
