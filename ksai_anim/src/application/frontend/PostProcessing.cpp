#include "advanced.h"
#include "vulkan/vulkan.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

static auto CreateShadowMapRenderPass(const vk::Device &dev) -> vk::RenderPass
{
    std::array<vk::AttachmentDescription, 2> Attachments;
    Attachments[0] = 
        vk::AttachmentDescription(vk::AttachmentDescriptionFlags(), vk::Format::eD32Sfloat)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
    ;

    vk::AttachmentReference DepthRef(0, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription Subpasses = vk::SubpassDescription(vk::SubpassDescriptionFlags())
        .setPDepthStencilAttachment(&DepthRef);

    vk::RenderPassCreateInfo RenderPassCreateInfo = vk::RenderPassCreateInfo(
        vk::RenderPassCreateFlags(),
        0,
        nullptr,
        1
    ).setSubpasses(Subpasses);

    return dev.createRenderPass(RenderPassCreateInfo);
}

extern "C" void PrepareForShadows(const vk_rsrs * rsrs, renderer_backend * inBackend)
{
    const vk::Device Device = vk_logical_device_;
    const vk::ImageCreateInfo ImageCreateInfo = vk::ImageCreateInfo(
        vk::ImageCreateFlags(),
        vk::ImageType::e2D,
        vk::Format::eD32Sfloat,
        vk::Extent3D(rsrs->vk_swap_chain_image_extent_2d_, 1),
        1,
        1,
        vk::SampleCountFlagBits::e1,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment bitand vk::ImageUsageFlagBits::eSampled,
        vk::SharingMode::eExclusive,
        0,
        nullptr,
        vk::ImageLayout::eUndefined,
        nullptr
    );

    inBackend->mShadow.mDepthImage = Device.createImage(ImageCreateInfo);

    const vk::ImageViewCreateInfo ImageViewCreateInfo = vk::ImageViewCreateInfo(
        vk::ImageViewCreateFlags(),
        inBackend->mShadow.mDepthImage,
        vk::ImageViewType::e2D,
        vk::Format::eD32Sfloat,
        vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB),
        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1)
    );

    inBackend->mShadow.mView = Device.createImageView(ImageViewCreateInfo);

    const glm::mat4 clip = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
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
    vk::FramebufferCreateInfo FrameBufferCreateInfo = vk::FramebufferCreateInfo()
        .setRenderPass(inBackend->mShadow.mRenderPass)
        .setAttachmentCount(1)
        .setAttachments(ShadowAttachment)
        .setWidth(rsrs->vk_swap_chain_image_extent_2d_.width)
        .setHeight(rsrs->vk_swap_chain_image_extent_2d_.height)
        .setLayers(1);

    inBackend->mShadow.mFrameBuffer = Device.createFramebuffer(FrameBufferCreateInfo);
}
