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

static auto CreatePostProcessingRenderPass(const vk_rsrs *rsrs, const vk::Device& dev)
{
	std::array<vk::AttachmentDescription, 2> Attachments;
	Attachments[0] = vk::AttachmentDescription()
		.setFormat(static_cast<vk::Format>(rsrs->vk_swap_chain_image_format_))
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eStore)
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

	Attachments[1] =
		vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), vk::Format::eD32Sfloat)
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eStore)
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

	vk::AttachmentReference ColorRef(0, vk::ImageLayout::eColorAttachmentOptimal);
	vk::AttachmentReference DepthRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::SubpassDescription Subpasses = vk::SubpassDescription(vk::SubpassDescriptionFlags())
		.setColorAttachmentCount(1)
		.setColorAttachments(ColorRef)
		.setPDepthStencilAttachment(&DepthRef);

	vk::RenderPassCreateInfo RenderPassCreateInfo = vk::RenderPassCreateInfo()
		.setSubpassCount(1)
		.setSubpasses(Subpasses)
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

		auto SamplerCreateInfo = vk::SamplerCreateInfo();
		inBackend->mShadow.mSampler = Device.createSampler(SamplerCreateInfo);

		auto clip = glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
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

		auto CmdCrtInfo = vk::CommandBufferAllocateInfo(rsrs->mRenderCommandPool, vk::CommandBufferLevel::ePrimary, 1);
		inBackend->mShadow.mCmdBuffer = Device.allocateCommandBuffers(CmdCrtInfo).front();

		vk::CommandBuffer cb = inBackend->mShadow.mCmdBuffer;

		auto ImgMemBarr = vk::ImageMemoryBarrier(
			vk::AccessFlagBits::eNone,
			vk::AccessFlagBits::eNone,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eShaderReadOnlyOptimal,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED,
			inBackend->mShadow.mDepthImage,
			vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth , 0U, 1U, 0U, 1U)
		);

		cb.begin(vk::CommandBufferBeginInfo());
		cb.pipelineBarrier(
			vk::PipelineStageFlagBits::eTopOfPipe,
			vk::PipelineStageFlagBits::eBottomOfPipe,
			vk::DependencyFlagBits::eByRegion,
			nullptr,
			nullptr,
			ImgMemBarr
		);

		cb.end();
		vk::Queue Gqueue = rsrs->vk_graphics_queue_;
		Gqueue.submit(vk::SubmitInfo().setCommandBufferCount(1).setCommandBuffers(cb));
		Gqueue.waitIdle();
	}

	void DestroyForShadows(vk_rsrs* rsrs, renderer_backend* backend)
	{
		auto Device = vk::Device(vk_logical_device_);
		Device.destroyImage(backend->mShadow.mDepthImage);
		Device.freeMemory(backend->mShadow.mDepthImageMemory);
		Device.destroyFramebuffer(backend->mShadow.mFrameBuffer);
		Device.destroyImageView(backend->mShadow.mView);
		Device.destroyRenderPass(backend->mShadow.mRenderPass);
		Device.destroySampler(backend->mShadow.mSampler);
		pipeline_vk_destroy3(&backend->mShadowPipe);
	}

	void DrawShadows(kie_Camera* camera, kie_Scene* scene, uint32_t viewport_obj_count, vk_rsrs* rsrs, renderer_backend* backend)
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
		vk::CommandBuffer CmdBuffer(backend->mShadow.mCmdBuffer);
		CmdBuffer.begin(vk::CommandBufferBeginInfo());
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
		CmdBuffer.end();
		auto SubmitInfo = vk::SubmitInfo().setCommandBufferCount(1).setCommandBuffers(CmdBuffer);
		vk::Queue Queue = rsrs->vk_graphics_queue_;
		Queue.submit(SubmitInfo);
		Queue.waitIdle();
	}

	void PrepareForPostProcessing(const vk_rsrs* rsrs, renderer_backend* inBackend)
	{
		vk::Device d(vk_logical_device_);
		auto ImageCreateInfo = vk::ImageCreateInfo(
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

		{
			inBackend->mPPDepth = d.createImage(ImageCreateInfo);
			vk::ImageMemoryRequirementsInfo2 info(inBackend->mPPDepth);
			auto MemoryRequirements = d.getImageMemoryRequirements2(info);
			auto MemoryAllocateInfo = vk::MemoryAllocateInfo(MemoryRequirements.memoryRequirements.size)
				.setMemoryTypeIndex(find_memory_type_util(MemoryRequirements.memoryRequirements.memoryTypeBits,
					static_cast<VkMemoryPropertyFlags>(vk::MemoryPropertyFlagBits::eDeviceLocal)));
			inBackend->mPPDepthMem = d.allocateMemory(MemoryAllocateInfo);
		}

		ImageCreateInfo
			.setFormat(static_cast<vk::Format>(rsrs->vk_swap_chain_image_format_))
			.setUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled);

		{
			inBackend->mPPColor = d.createImage(ImageCreateInfo);
			vk::ImageMemoryRequirementsInfo2 info(inBackend->mPPColor);
			auto MemoryRequirements = d.getImageMemoryRequirements2(info);
			auto MemoryAllocateInfo = 
				vk::MemoryAllocateInfo(MemoryRequirements.memoryRequirements.size)
				.setMemoryTypeIndex(find_memory_type_util(MemoryRequirements.memoryRequirements.memoryTypeBits,
					static_cast<VkMemoryPropertyFlags>(vk::MemoryPropertyFlagBits::eDeviceLocal)));
			inBackend->mPPColorMem = d.allocateMemory(MemoryAllocateInfo);
		}
		
		d.bindImageMemory(inBackend->mPPColor, inBackend->mPPColorMem, 0);
		d.bindImageMemory(inBackend->mPPDepth, inBackend->mPPDepthMem, 0);

		auto ImageViewCreateInfo = vk::ImageViewCreateInfo(
			vk::ImageViewCreateFlags(),
			inBackend->mPPDepth,
			vk::ImageViewType::e2D,
			vk::Format::eD32Sfloat,
			vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB),
			vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1)
		);

		inBackend->mPPDepthView = d.createImageView(ImageViewCreateInfo);
		ImageViewCreateInfo
			.setImage(inBackend->mPPColor)
			.setFormat(static_cast<vk::Format>(rsrs->vk_swap_chain_image_format_))
			.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
		inBackend->mPPColorView = d.createImageView(ImageViewCreateInfo);

	
		inBackend->mPPRenderPass = CreatePostProcessingRenderPass(rsrs, d);

		auto Attachments = std::array<vk::ImageView, 2>{inBackend->mPPColorView, inBackend->mPPDepthView};
		auto FrameBufferCreateInfo = vk::FramebufferCreateInfo()
			.setRenderPass(inBackend->mPPRenderPass)
			.setAttachmentCount(2)
			.setAttachments(Attachments)
			.setWidth(rsrs->vk_swap_chain_image_extent_2d_.width)
			.setHeight(rsrs->vk_swap_chain_image_extent_2d_.height)
			.setLayers(1);
		inBackend->mPPFrameBuffer = d.createFramebuffer(FrameBufferCreateInfo);

		FrameBufferCreateInfo = vk::FramebufferCreateInfo()
			.setRenderPass(rsrs->vk_render_pass_)
			.setAttachmentCount(2)
			.setAttachments(Attachments)
			.setWidth(rsrs->vk_swap_chain_image_extent_2d_.width)
			.setHeight(rsrs->vk_swap_chain_image_extent_2d_.height)
			.setLayers(1);
		inBackend->mBeforePPFrameBuffer = d.createFramebuffer(FrameBufferCreateInfo);



		std::array<vk::DescriptorPoolSize, KSAI_VK_DESCRIPTOR_POOL_SIZE> PoolSizes;
		{
			int i = 0;
			for (auto& PoolSize : PoolSizes)
			{
				PoolSize = inBackend->pool_sizes[i];
				i++;
			}
		}
		auto DpoolCreateInfo = vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlags(), 2, PoolSizes);
		inBackend->mPPDpool = d.createDescriptorPool(DpoolCreateInfo);

		std::array<vk::DescriptorSetLayout, 1> DescriptorSetLayouts = { inBackend->mShadowPipe.vk_descriptor_set_layout_ };
		auto DsetAllocateInfo = vk::DescriptorSetAllocateInfo(inBackend->mPPDpool, DescriptorSetLayouts);
		inBackend->mPPDset = d.allocateDescriptorSets(DsetAllocateInfo).front();

		inBackend->mPostSampler = d.createSampler(vk::SamplerCreateInfo());

		auto imageInfo = vk::DescriptorImageInfo(inBackend->mShadow.mSampler, inBackend->mShadow.mView, vk::ImageLayout::eShaderReadOnlyOptimal);// TODO
		auto WriteDescriptorSet = vk::WriteDescriptorSet(inBackend->mPPDset, 1, 0, vk::DescriptorType::eCombinedImageSampler, imageInfo);

		auto imageInfo2 = vk::DescriptorImageInfo(inBackend->mPostSampler, inBackend->mPPColorView, vk::ImageLayout::eShaderReadOnlyOptimal);// TODO
		auto WriteDescriptorSet2 = vk::WriteDescriptorSet(inBackend->mPPDset, 2, 0, vk::DescriptorType::eCombinedImageSampler, imageInfo2);
		std::array<vk::WriteDescriptorSet, 2> wdsets = { WriteDescriptorSet, WriteDescriptorSet2};
		d.updateDescriptorSets(wdsets, nullptr);

		std::array<kie_Vertex, 4> vertices{};
		vertices[0] = (kie_Vertex){ .position = {1.0f,  1.0f, 0.0f}, .tex_coord = {1.0f, 1.0f} };
		vertices[1] = (kie_Vertex){ .position = {1.0f,  -1.0f, 0.0f}, .tex_coord = {1.0f, 0.0f} };
		vertices[2] = (kie_Vertex){ .position = {-1.0f, -1.0f, 0.0f}, .tex_coord = {0.0f, 0.0f} };
		vertices[3] = (kie_Vertex){ .position = {-1.0f,  1.0f, 0.0f}, .tex_coord = {0.0f, 1.0f} };
		std::array<uint32_t, 6> indices = {
			0, 1, 3,
			1, 2, 3
		};

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
		void* vdata = Device.mapMemory(inBackend->mScreenQuadVmem, 0, vsize);
		std::memcpy(vdata, vertices.data(), vsize);
		Device.unmapMemory(inBackend->mScreenQuadVmem);

		vsize = indices.size() * sizeof(uint32_t);
		create_buffer_util(
			vsize,
			static_cast<VkBufferUsageFlags>(vk::BufferUsageFlagBits::eIndexBuffer),
			static_cast<VkMemoryPropertyFlags>(vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible),
			&inBackend->mScreenQuadBufferI,
			&inBackend->mScreenQuadImem,
			Device
		);
		void* idata = Device.mapMemory(inBackend->mScreenQuadImem, 0, vsize);
		std::memcpy(idata, indices.data(), vsize);
		Device.unmapMemory(inBackend->mScreenQuadImem);

		inBackend->mPPCmdBuffer = Device.allocateCommandBuffers(vk::CommandBufferAllocateInfo(vk_command_pool_, vk::CommandBufferLevel::ePrimary, 1)).front();

		VulkanPipeline::createPipelineForPP(rsrs, inBackend, &inBackend->mPPPipeline, inBackend->mPPRenderPass);
	}

	void DestroyForPostProcessing(const vk_rsrs* rsr, renderer_backend* backend)
	{
		vk::Device d(vk_logical_device_);
		d.freeMemory(backend->mScreenQuadImem);
		d.freeMemory(backend->mScreenQuadVmem);
		d.destroyBuffer(backend->mScreenQuadBufferV);
		d.destroyBuffer(backend->mScreenQuadBufferI);
		d.destroyDescriptorPool(backend->mPPDpool);
		d.destroyImage(backend->mPPColor);
		d.destroyImage(backend->mPPDepth);
		d.freeMemory(backend->mPPColorMem);
		d.freeMemory(backend->mPPDepthMem);
		d.destroyImageView(backend->mPPColorView);
		d.destroyImageView(backend->mPPDepthView);
		d.destroyRenderPass(backend->mPPRenderPass);
		d.destroySampler(backend->mPostSampler);
		d.destroyFramebuffer(backend->mPPFrameBuffer);
		d.destroyFramebuffer(backend->mBeforePPFrameBuffer);
		pipeline_vk_destroy3(&backend->mPPPipeline);
	}

	void DrawForPostProcessed(kie_Camera* camera, kie_Scene* scene, uint32_t viewport_obj_count, vk_rsrs* rsrs, renderer_backend* backend)
	{
		vk::Device dev = vk_logical_device_;
		dev.waitIdle();

		vk::CommandBuffer Cmd(backend->mPPCmdBuffer);
		Cmd.begin(vk::CommandBufferBeginInfo());
		auto ClearValue = std::array<vk::ClearValue, 2>{};
		ClearValue.fill(
			vk::ClearValue()
			.setDepthStencil(vk::ClearDepthStencilValue(1.0f, 0.0f))
			.setColor(vk::ClearColorValue(1.0f, 1.0f, 0.0f, 0.0f))
			);
		Cmd.beginRenderPass(
			vk::RenderPassBeginInfo(
				rsrs->vk_render_pass_,
				backend->mBeforePPFrameBuffer,
				vk::Rect2D(vk::Offset2D(0, 0), rsrs->vk_swap_chain_image_extent_2d_),
				ClearValue
			), vk::SubpassContents::eInline);
		threeD_viewport_draw_buf_without_viewport_and_lights(camera, scene, backend, rsrs, 4, Cmd, 1);
		Cmd.endRenderPass();
		Cmd.end();

		vk::Queue gq = rsrs->vk_graphics_queue_;
		gq.submit(vk::SubmitInfo().setCommandBufferCount(1).setCommandBuffers(Cmd));
		gq.waitIdle();

		Cmd.begin(vk::CommandBufferBeginInfo());
		ClearValue.fill(vk::ClearValue()
			.setDepthStencil(vk::ClearDepthStencilValue(1.0f, 0.0f))
			.setColor(vk::ClearColorValue(1.0f, 1.0f, 1.0f, 1.0f)));
		Cmd.beginRenderPass(
			vk::RenderPassBeginInfo(
				backend->mPPRenderPass,
				backend->mPPFrameBuffer, // TODO
				vk::Rect2D(vk::Offset2D(0, 0), rsrs->vk_swap_chain_image_extent_2d_),
				ClearValue
			), vk::SubpassContents::eInline);

		Cmd.setDepthTestEnable(VK_FALSE);
		Cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, backend->mPPPipeline.vk_pipeline_);
		std::array<vk::DescriptorSet, 1> dsets = {backend->mPPDset};
		std::array<uint32_t, 0> offsets{};
		Cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, backend->mPPPipeline.vk_pipeline_layout_, 0, dsets, offsets);

		auto viewport = vk::Viewport()
			.setHeight(rsrs->vk_swap_chain_image_extent_2d_.height)
			.setWidth(rsrs->vk_swap_chain_image_extent_2d_.width)
			.setMinDepth(0.0f)
			.setMaxDepth(1.0f);
		Cmd.setViewport(0, viewport);

		auto scissor = vk::Rect2D(vk::Offset2D(0, 0), rsrs->vk_swap_chain_image_extent_2d_);
		Cmd.setScissor(0, scissor);

		std::array < vk::Buffer, 1> Vbuffers = { backend->mScreenQuadBufferV };
		std::array<vk::DeviceSize, 1> Offsets = { 0 };
		Cmd.bindVertexBuffers(0, Vbuffers, Offsets );
		Cmd.bindIndexBuffer(backend->mScreenQuadBufferI, 0, vk::IndexType::eUint32);
		Cmd.drawIndexed(6, 1, 0, 0, 0);
		Cmd.setDepthTestEnable(VK_FALSE);
		Cmd.endRenderPass();
		Cmd.end();
		vk::Queue Q = rsrs->vk_graphics_queue_;
		Q.submit(vk::SubmitInfo(nullptr, nullptr, Cmd));
		Q.waitIdle();
	}
}
