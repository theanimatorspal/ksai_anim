#define VK_USE_PLATFORM_WIN32_KHR

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ksai/ksai.h>
#include <vendor/cglm/cglm.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include "utils.h"
#include "init.h"
#include "cleanup.h"
#include <stdbool.h>
#include <string.h>

#ifdef NDEBUG
const int enable_validation_layers_ = 0;
#else
const int enable_validation_layers_ = 1;
#endif
#define NO_OF_ENABLED_EXTENSION  1
const char *device_extensions_[] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
#define NO_OF_ENABLED_LAYERS 1
const char *validation_layers_[] = {
	"VK_LAYER_KHRONOS_validation"
};
char **extensions_ = { 0 };
int no_of_extensions_ = 0;

VkInstance vk_instance_;
VkPhysicalDevice vk_physical_device_;
VkDevice vk_logical_device_;
VkCommandPool vk_command_pool_;
VkCommandBuffer vk_command_buffer_[MAX_FRAMES_IN_FLIGHT];



extern VkCommandPool vk_command_pool_;
extern VkCommandBuffer vk_command_buffer_[MAX_FRAMES_IN_FLIGHT];


void create_window_surface(SDL_Window *_window, vk_rsrs *_rsrs, bool first_time, VkInstance instance)
{
	if (first_time == false)
	{
		cleanup_swap_chain(_rsrs);
		vkDestroySurfaceKHR(instance, _rsrs->vk_srf, NULL);
		KSAI_VK_ASSERT(SDL_Vulkan_CreateSurface(_window, instance, &_rsrs->vk_srf));
		create_swap_chain_image_views_render_pass(_rsrs);
	}
	else
	{
		KSAI_SDL_ASSERT(SDL_Vulkan_CreateSurface(_window, instance, &_rsrs->vk_srf));
	}

}

void create_vulkan_render_pass(vk_rsrs *_rsrs)
{
	/* Create Render Pass*/
	{

		VkAttachmentDescription depth_attachment = (VkAttachmentDescription){
			.format = _rsrs->vk_depth_image_format,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};

		VkAttachmentReference depth_attachment_ref = (VkAttachmentReference)
		{
			.attachment = 1,
			.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};


		VkAttachmentDescription color_attachment = { 0 };
		color_attachment.format = _rsrs->vk_swap_chain_image_format_;
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		/* IMAGES NEED TO BE TRANSITIONED TO SPECIFIC LAYOUT */

		VkAttachmentReference color_attachment_ref = { 0 };
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = { 0 };
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_ref;
		subpass.pDepthStencilAttachment = &depth_attachment_ref;

		VkSubpassDescription subpass_des[2] = { subpass };
		VkAttachmentDescription attch_des[2] = { color_attachment, depth_attachment };

		VkRenderPassCreateInfo render_pass_info = { 0 };
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_info.attachmentCount = 2;
		render_pass_info.pAttachments = attch_des;
		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = subpass_des;

		VkSubpassDependency implicit_dependency = { 0 };
		implicit_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		implicit_dependency.dstSubpass = 0;
		implicit_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		implicit_dependency.srcAccessMask = 0;
		implicit_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		implicit_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		implicit_dependency.dependencyFlags = 0;

		VkSubpassDependency dependency = { 0 };
		dependency.srcSubpass = 0;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = 0;
		dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkSubpassDependency dependencies[2] = { implicit_dependency, dependency };

		render_pass_info.dependencyCount = 2;
		render_pass_info.pDependencies = dependencies;


		if (vkCreateRenderPass(vk_logical_device_, &render_pass_info, NULL, &_rsrs->vk_render_pass_) != VK_SUCCESS)
		{
			printf("Failed to create render pass\n");
		}
	}

	/* Create Frame Buffers */
	{
		for (int i = 0; i < _rsrs->no_of_swap_chain_images_; i++)
		{
			VkImageView attachments[2] = { _rsrs->vk_swap_chain_image_views_[i], _rsrs->vk_depth_image_view };
			VkFramebufferCreateInfo framebuffer_info = { 0 };
			framebuffer_info = (VkFramebufferCreateInfo){
				.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.renderPass = _rsrs->vk_render_pass_,
				.attachmentCount = 2,
				.pAttachments = attachments,
				.width = _rsrs->vk_swap_chain_image_extent_2d_.width,
				.height = _rsrs->vk_swap_chain_image_extent_2d_.height,
				.layers = 1
			};

			if (vkCreateFramebuffer(vk_logical_device_, &framebuffer_info, NULL, &_rsrs->vk_swap_chain_frame_buffers_[i]) != VK_SUCCESS)
			{
				printf("Failed to create frame buffer\n");
			}
		}
	}

}

void pick_physical_and_logical_devices(vk_rsrs *_rsrs, VkInstance instance)
{
	static bool phy_and_log_dev_cr = false;
	{
		static queue_family_indices indices_xt;
		static swap_chain_support_details swap_chain_support_xt;
		/* Pick Physical Device*/
		{
			uint32_t device_count = 0;
			vkEnumeratePhysicalDevices(instance, &device_count, NULL);

			if (device_count == 0)
			{
				printf("No devices_xt supporting vulkan\n");
			}

			static VkPhysicalDevice devices_xt[32];
			vkEnumeratePhysicalDevices(instance, &device_count, devices_xt);

			for (int i = 0; i < device_count; i++)
			{
				/* Query Extension Support */
				uint32_t extension_count;
				static VkExtensionProperties available_extensions_xt[512];
				vkEnumerateDeviceExtensionProperties(devices_xt[i], NULL, &extension_count, NULL);
				vkEnumerateDeviceExtensionProperties(devices_xt[i], NULL, &extension_count, available_extensions_xt);
				int extension_supported = 0;
				for (int i = 0; i < NO_OF_ENABLED_EXTENSION; i++)
				{
					for (int j = 0; j < extension_count; j++)
					{
						if (strcmp(available_extensions_xt[j].extensionName, device_extensions_[i]) == 0)
						{
							extension_supported = 1;
							break;
						}
					}
				}

				/* Find Queue Families */
				indices_xt = find_queue_families_util(devices_xt[i]);

				/* Check swap chain support */
				int swap_chain_adequate = 0;
				if (extension_supported)
				{

					vkGetPhysicalDeviceSurfaceCapabilitiesKHR(devices_xt[i], _rsrs->vk_srf, &swap_chain_support_xt.capabilities);

					vkGetPhysicalDeviceSurfaceFormatsKHR(devices_xt[i], _rsrs->vk_srf, (uint32_t *) &swap_chain_support_xt.formats_size, NULL);

					if (swap_chain_support_xt.formats_size != 0)
					{
						vkGetPhysicalDeviceSurfaceFormatsKHR(devices_xt[i], _rsrs->vk_srf, (uint32_t *) &swap_chain_support_xt.formats_size, swap_chain_support_xt.formats);
					}

					vkGetPhysicalDeviceSurfacePresentModesKHR(devices_xt[i], _rsrs->vk_srf, (uint32_t *) &swap_chain_support_xt.present_modes_size, NULL);

					if (swap_chain_support_xt.present_modes_size != 0)
					{
						vkGetPhysicalDeviceSurfacePresentModesKHR(devices_xt[i], _rsrs->vk_srf, (uint32_t *) &swap_chain_support_xt.present_modes_size, swap_chain_support_xt.present_modes);
					}

					//swap_chain_adequate = (swap_chain_support_xt.formats != NULL) && (swap_chain_support_xt.present_modes != NULL);
					swap_chain_adequate = true;
				}

				if (extension_supported && swap_chain_adequate)
				{
					VkPhysicalDeviceFeatures features;
					vkGetPhysicalDeviceFeatures(devices_xt[i], &features);
					if (features.geometryShader == VK_TRUE)
					{
						vk_physical_device_ = devices_xt[i];
					}
					else
					{
						printf("NO Geometry shader supporting Device");
					}
				}
			}

			if (vk_physical_device_ == VK_NULL_HANDLE)
			{
				printf("No fucking GPU\n");
			}

		}

		/* Create Logical Device */
		indices_xt = find_queue_families_util(vk_physical_device_);
		VkDeviceQueueCreateInfo queue_create_infos[2] = { 0 };
		uint32_t unique_queue_families[2] = { indices_xt.graphics_family, indices_xt.present_family };
		{
			for (int i = 0; i < 2; i++)
			{
				queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queue_create_infos[i].queueFamilyIndex = unique_queue_families[i];
				queue_create_infos[i].queueCount = 1;
				/* EUTA VANDA BADHI CHAIDAINA */
				/*
					Vulkan le chae queue ko priority to influence scheduling of command buffer execution
					represent between 0 and 1
				*/
				float queue_priority = 1.0f;
				queue_create_infos[i].pQueuePriorities = &queue_priority;
				/* AHILE CHAHIDAINA */
			}

			VkPhysicalDeviceFeatures device_features = { 0 };
			device_features = (VkPhysicalDeviceFeatures)
			{
				.samplerAnisotropy = VK_TRUE,
				.geometryShader = VK_TRUE,
				.fillModeNonSolid = VK_TRUE,
				.imageCubeArray = VK_TRUE
			};

			VkDeviceCreateInfo create_info = { 0 };
			create_info = (VkDeviceCreateInfo){
				.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
				.pQueueCreateInfos = (VkDeviceQueueCreateInfo *) &queue_create_infos,
				.queueCreateInfoCount = 2,
				.pEnabledFeatures = &device_features,
				.enabledExtensionCount = NO_OF_ENABLED_EXTENSION,
				.ppEnabledExtensionNames = device_extensions_
			};

			if (enable_validation_layers_)
			{
				create_info.enabledLayerCount = NO_OF_ENABLED_LAYERS;
				create_info.ppEnabledLayerNames = validation_layers_;
			}
			else
			{
				create_info.enabledLayerCount = 0;
			}


			if (phy_and_log_dev_cr == false)
				if (vkCreateDevice(vk_physical_device_, &create_info, NULL, &vk_logical_device_) != VK_SUCCESS)
				{
					printf("Failed to Create logical device\n");
				}
			vkGetDeviceQueue(vk_logical_device_, indices_xt.graphics_family, 0, &_rsrs->vk_graphics_queue_);
			vkGetDeviceQueue(vk_logical_device_, indices_xt.present_family, 0, &_rsrs->vk_present_queue_);
		}

		phy_and_log_dev_cr = true;

		/* Create Swap Chain */
		{
			VkSurfaceFormatKHR surface_format;
			surface_format = swap_chain_support_xt.formats[0];
			for (int i = 0; i < swap_chain_support_xt.formats_size; i++)
			{
				if (swap_chain_support_xt.formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && swap_chain_support_xt.formats[i].colorSpace == VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT)
				{
					surface_format = swap_chain_support_xt.formats[i];
				}
			}

			VkPresentModeKHR present_mode;
			present_mode = VK_PRESENT_MODE_FIFO_KHR;
			for (int i = 0; i < swap_chain_support_xt.present_modes_size; i++)
			{
				if (swap_chain_support_xt.present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					present_mode = swap_chain_support_xt.present_modes[i];
				}
			}

			VkExtent2D extent;
			if (swap_chain_support_xt.capabilities.currentExtent.width != KSAI_U64_MAX)
			{
				extent = swap_chain_support_xt.capabilities.currentExtent;
			}
			else
			{
				/* mAC wala dhani harko laagi*/
				//int width, height;
				//glfwGetFramebufferSize(_wind, &width, &height);
				VkExtent2D actual_extent = { .width = 0, .height = 0 };
				actual_extent.width = glm_clamp(actual_extent.width, swap_chain_support_xt.capabilities.minImageExtent.width, swap_chain_support_xt.capabilities.maxImageExtent.width);
				actual_extent.width = glm_clamp(actual_extent.height, swap_chain_support_xt.capabilities.minImageExtent.height, swap_chain_support_xt.capabilities.maxImageExtent.height);
				extent = actual_extent;
				// FOR MAC WALA DHANIS
			}

			uint32_t image_count = swap_chain_support_xt.capabilities.minImageCount + 1;

			/*
				sticking to this minimum means we do not have anyy buffer to hold another
				image, so we have to wait for the driver so that it completes its stuff
			*/
			if (swap_chain_support_xt.capabilities.maxImageCount > 0 && image_count > swap_chain_support_xt.capabilities.maxImageCount)
			{
				image_count = swap_chain_support_xt.capabilities.maxImageCount;
			}

			VkSwapchainCreateInfoKHR create_info = { 0 };
			create_info = (VkSwapchainCreateInfoKHR){
				.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
				.surface = _rsrs->vk_srf,
				.minImageCount = image_count,
				.imageFormat = surface_format.format,
				.imageColorSpace = surface_format.colorSpace,
				.imageExtent = extent,
				.imageArrayLayers = 1,
				/*SPECIFIES THE AMOUN OF LAYER EACH IMAGE CONSISTS OF  ALWAYS ONE EXCEPT FOR STEREOSCOPIC 3d app*/
				.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				/* Rendering directly to the images */
				/* For using post processing */
				/*VK_IMAGE_USAGE_TRANSFER_DST_BIT*/
			};

			uint32_t queue_family_indices[] = { indices_xt.graphics_family, indices_xt.present_family };

			if (indices_xt.graphics_family != indices_xt.present_family)
			{
				create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				create_info.queueFamilyIndexCount = 2;
				create_info.pQueueFamilyIndices = queue_family_indices;
			}
			else
			{
				create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				create_info.queueFamilyIndexCount = 0;
				create_info.pQueueFamilyIndices = NULL;
			}

			create_info.preTransform = swap_chain_support_xt.capabilities.currentTransform;
			/* NO TRANSFORMATIONS WANTED */
			create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			/* With window manager*/
			create_info.presentMode = present_mode;
			create_info.clipped = VK_TRUE;
			/* Choppiyekko pixels haru ko laagi clip garne ki na garne vanne kuro */
			create_info.oldSwapchain = VK_NULL_HANDLE;

			if (vkCreateSwapchainKHR(vk_logical_device_, &create_info, NULL, &_rsrs->vk_swap_chain_) != VK_SUCCESS)
			{
				printf("Swapchain createion failed\n");
			}

			vkGetSwapchainImagesKHR(vk_logical_device_, _rsrs->vk_swap_chain_, &_rsrs->no_of_swap_chain_images_, NULL);
			vkGetSwapchainImagesKHR(vk_logical_device_, _rsrs->vk_swap_chain_, &_rsrs->no_of_swap_chain_images_, _rsrs->vk_swap_chain_images_);

			_rsrs->vk_swap_chain_image_format_ = surface_format.format;
			_rsrs->vk_swap_chain_image_extent_2d_ = extent;
		}



		/* Create Image Views */
		{
			for (int i = 0; i < _rsrs->no_of_swap_chain_images_; i++)
			{
				VkImageViewCreateInfo create_info = { 0 };
				create_info = (VkImageViewCreateInfo){
					.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
					.image = _rsrs->vk_swap_chain_images_[i],
					.viewType = VK_IMAGE_VIEW_TYPE_2D,
					.format = _rsrs->vk_swap_chain_image_format_,
					.components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
					.components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
					.components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
					.components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
					.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.subresourceRange.baseMipLevel = 0,
					.subresourceRange.levelCount = 1,
					.subresourceRange.baseArrayLayer = 0,
					.subresourceRange.layerCount = 1
				};

				if (vkCreateImageView(vk_logical_device_, &create_info, NULL, &_rsrs->vk_swap_chain_image_views_[i]) != VK_SUCCESS)
				{
					printf("Failed to create image views !\n");
				}
			}
		}



		/* Create Command Pool */
		{
			queue_family_indices indcs = find_queue_families_util(vk_physical_device_);
			VkCommandPoolCreateInfo pool_info = { 0 };
			pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			pool_info.queueFamilyIndex = indcs.graphics_family;

			if (vkCreateCommandPool(vk_logical_device_, &pool_info, NULL, &vk_command_pool_) != VK_SUCCESS)
			{
				printf("Failed to create command pool\n");
			}
		}


		/* Create Depth Image View */
		{
			_rsrs->vk_depth_image_format = VK_FORMAT_D32_SFLOAT;
			VkFormat format = _rsrs->vk_depth_image_format;
			create_image_util(
				_rsrs->vk_swap_chain_image_extent_2d_.width,
				_rsrs->vk_swap_chain_image_extent_2d_.height,
				format,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&_rsrs->vk_depth_image,
				&_rsrs->vk_depth_image_memory, vk_logical_device_
			);
			_rsrs->vk_depth_image_view = create_image_view_util2(_rsrs->vk_depth_image, format, VK_IMAGE_ASPECT_DEPTH_BIT);
			transition_image_layout_util(
				_rsrs->vk_depth_image,
				format,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				vk_command_pool_,
				_rsrs->vk_graphics_queue_
			);
		}



		/* Create Command buffers */
		{
			VkCommandBufferAllocateInfo allocate_info = { 0 };
			allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocate_info.commandPool = vk_command_pool_;
			allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocate_info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

			if (vkAllocateCommandBuffers(vk_logical_device_, &allocate_info, vk_command_buffer_) != VK_SUCCESS)
			{
				printf("Failed to create command buffers\n");
			}
		}

	}
}

void create_vulkan_instace(SDL_Window *window, VkInstance *instance)
{
	/* Check Validation Layer Support */
	{
		uint32_t layer_count;
		vkEnumerateInstanceLayerProperties(&layer_count, NULL);

		static VkLayerProperties available_layers_xt[20];
		vkEnumerateInstanceLayerProperties(&layer_count, available_layers_xt);

		int layer_found = 0;
		for (int i = 0; i < NO_OF_ENABLED_LAYERS; i++)
		{
			for (int j = 0; j < layer_count; j++)
			{
				if (strcmp(available_layers_xt[j].layerName, validation_layers_[i]) == 0)
				{
					layer_found = 1;
					break;
				}
			}
		}

		if (!layer_found && enable_validation_layers_)
		{
			printf("Validation Layers requested but not found \n");
		}
	}


	/* Create Vulkan Instance */
	{
		VkApplicationInfo app_info = { 0 };
		app_info = (VkApplicationInfo){
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = "plugin",
			.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
			.pEngineName = "No Engine",
			.engineVersion = VK_MAKE_VERSION(1, 0, 0),
			.apiVersion = VK_API_VERSION_1_3,
			.pNext = NULL
		};

		/* Get Required Extensions */
		{
			uint32_t glfwExtensionCount = 0;
			SDL_Vulkan_GetInstanceExtensions(window, &glfwExtensionCount, NULL);
			no_of_extensions_ = glfwExtensionCount;
			extensions_ = (char **) malloc(sizeof(char *) * (no_of_extensions_ + 1));

			SDL_Vulkan_GetInstanceExtensions(window, &glfwExtensionCount, (const char **) extensions_);

			if (enable_validation_layers_)
			{
				extensions_[no_of_extensions_] = (char *) malloc(sizeof(VK_EXT_DEBUG_UTILS_EXTENSION_NAME));
				strcpy_s(extensions_[no_of_extensions_], sizeof(extensions_[0][0]) * sizeof(VK_EXT_DEBUG_UTILS_EXTENSION_NAME), VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
				no_of_extensions_++;
			}
		}

		VkInstanceCreateInfo create_info = { 0 };
		create_info = (VkInstanceCreateInfo)
		{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &app_info,
			.enabledExtensionCount = no_of_extensions_,
			.ppEnabledExtensionNames = extensions_
		};

		if (enable_validation_layers_)
		{
			create_info.enabledLayerCount = NO_OF_ENABLED_LAYERS;
			create_info.ppEnabledLayerNames = validation_layers_;
		}
		else
		{
			create_info.enabledLayerCount = 0;
		}

		KSAI_VK_ASSERT(vkCreateInstance(&create_info, NULL, instance));

	}


}

void create_swap_chain_image_views_render_pass(vk_rsrs *_rsrs)
{
	static queue_family_indices indices_xt;
	indices_xt = find_queue_families_util(vk_physical_device_);
	static swap_chain_support_details swap_chain_support_xt;

	/* Checkout swap_chain_capabilities */
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_physical_device_, _rsrs->vk_srf, &swap_chain_support_xt.capabilities);

		vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physical_device_, _rsrs->vk_srf, (uint32_t *) &swap_chain_support_xt.formats_size, NULL);

		if (swap_chain_support_xt.formats_size != 0)
		{
			vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physical_device_, _rsrs->vk_srf, (uint32_t *) &swap_chain_support_xt.formats_size, swap_chain_support_xt.formats);
		}

		vkGetPhysicalDeviceSurfacePresentModesKHR(vk_physical_device_, _rsrs->vk_srf, (uint32_t *) &swap_chain_support_xt.present_modes_size, NULL);

		if (swap_chain_support_xt.present_modes_size != 0)
		{
			vkGetPhysicalDeviceSurfacePresentModesKHR(vk_physical_device_, _rsrs->vk_srf, (uint32_t *) &swap_chain_support_xt.present_modes_size, swap_chain_support_xt.present_modes);
		}
	}

	/* Create Swap Chain */
	{
		VkSurfaceFormatKHR surface_format;
		surface_format = swap_chain_support_xt.formats[0];
		for (int i = 0; i < swap_chain_support_xt.formats_size; i++)
		{
			if (swap_chain_support_xt.formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && swap_chain_support_xt.formats[i].colorSpace == VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT)
			{
				surface_format = swap_chain_support_xt.formats[i];
			}
		}

		VkPresentModeKHR present_mode;
		present_mode = VK_PRESENT_MODE_FIFO_KHR;
		for (int i = 0; i < swap_chain_support_xt.present_modes_size; i++)
		{
			if (swap_chain_support_xt.present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				present_mode = swap_chain_support_xt.present_modes[i];
			}
		}

		VkExtent2D extent;
		if (swap_chain_support_xt.capabilities.currentExtent.width != KSAI_U64_MAX)
		{
			extent = swap_chain_support_xt.capabilities.currentExtent;
		}
		else
		{
			/* mAC wala dhani harko laagi*/
			//int width, height;
			//glfwGetFramebufferSize(external_window, &width, &height);
			VkExtent2D actual_extent = { .width = 0, .height = 0 };
			actual_extent.width = glm_clamp(actual_extent.width, swap_chain_support_xt.capabilities.minImageExtent.width, swap_chain_support_xt.capabilities.maxImageExtent.width);
			actual_extent.width = glm_clamp(actual_extent.height, swap_chain_support_xt.capabilities.minImageExtent.height, swap_chain_support_xt.capabilities.maxImageExtent.height);
			extent = actual_extent;
			// NOT FOR  MACSS
		}

		uint32_t image_count = swap_chain_support_xt.capabilities.minImageCount + 1;

		/*
			sticking to this minimum means we do not have anyy buffer to hold another
			image, so we have to wait for the driver so that it completes its stuff
		*/
		if (swap_chain_support_xt.capabilities.maxImageCount > 0 && image_count > swap_chain_support_xt.capabilities.maxImageCount)
		{
			image_count = swap_chain_support_xt.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR create_info = { 0 };
		create_info = (VkSwapchainCreateInfoKHR){
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = _rsrs->vk_srf,
			.minImageCount = image_count,
			.imageFormat = surface_format.format,
			.imageColorSpace = surface_format.colorSpace,
			.imageExtent = extent,
			.imageArrayLayers = 1,
			/*SPECIFIES THE AMOUN OF LAYER EACH IMAGE CONSISTS OF  ALWAYS ONE EXCEPT FOR STEREOSCOPIC 3d app*/
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			/* Rendering directly to the images */
			/* For using post processing */
			/*VK_IMAGE_USAGE_TRANSFER_DST_BIT*/
		};

		uint32_t queue_family_indices[] = { indices_xt.graphics_family, indices_xt.present_family };

		if (indices_xt.graphics_family != indices_xt.present_family)
		{
			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			create_info.queueFamilyIndexCount = 2;
			create_info.pQueueFamilyIndices = queue_family_indices;
		}
		else
		{
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			create_info.queueFamilyIndexCount = 0;
			create_info.pQueueFamilyIndices = NULL;
		}

		create_info.preTransform = swap_chain_support_xt.capabilities.currentTransform;
		/* NO TRANSFORMATIONS WANTED */
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		/* With window manager*/
		create_info.presentMode = present_mode;
		create_info.clipped = VK_TRUE;
		/* Choppiyekko pixels haru ko laagi clip garne ki na garne vanne kuro */
		create_info.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(vk_logical_device_, &create_info, NULL, &_rsrs->vk_swap_chain_) != VK_SUCCESS)
		{
			printf("Swapchain createion failed\n");
		}

		vkGetSwapchainImagesKHR(vk_logical_device_, _rsrs->vk_swap_chain_, &_rsrs->no_of_swap_chain_images_, NULL);
		vkGetSwapchainImagesKHR(vk_logical_device_, _rsrs->vk_swap_chain_, &_rsrs->no_of_swap_chain_images_, _rsrs->vk_swap_chain_images_);

		_rsrs->vk_swap_chain_image_format_ = surface_format.format;
		_rsrs->vk_swap_chain_image_extent_2d_ = extent;

		_rsrs->vk_depth_image_format = VK_FORMAT_D32_SFLOAT;
		VkFormat format = _rsrs->vk_depth_image_format;
		create_image_util(
			_rsrs->vk_swap_chain_image_extent_2d_.width,
			_rsrs->vk_swap_chain_image_extent_2d_.height,
			format,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&_rsrs->vk_depth_image,
			&_rsrs->vk_depth_image_memory, vk_logical_device_
		);
	}


	/* Create Image Views */
	{
		for (int i = 0; i < _rsrs->no_of_swap_chain_images_; i++)
		{
			VkImageViewCreateInfo create_info = { 0 };
			create_info = (VkImageViewCreateInfo){
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = _rsrs->vk_swap_chain_images_[i],
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = _rsrs->vk_swap_chain_image_format_,
				.components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
				.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.subresourceRange.baseMipLevel = 0,
				.subresourceRange.levelCount = 1,
				.subresourceRange.baseArrayLayer = 0,
				.subresourceRange.layerCount = 1
			};

			if (vkCreateImageView(vk_logical_device_, &create_info, NULL, &_rsrs->vk_swap_chain_image_views_[i]) != VK_SUCCESS)
			{
				printf("Failed to create image views !\n");
			}
		}
	}


	/* Create Depth Image View */
	{
		VkImageViewCreateInfo create_info = { 0 };
		create_info = (VkImageViewCreateInfo){
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = _rsrs->vk_depth_image,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = _rsrs->vk_depth_image_format,
			.components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
			.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
			.subresourceRange.baseMipLevel = 0,
			.subresourceRange.levelCount = 1,
			.subresourceRange.baseArrayLayer = 0,
			.subresourceRange.layerCount = 1
		};
		KSAI_VK_ASSERT(vkCreateImageView(vk_logical_device_, &create_info, NULL, &_rsrs->vk_depth_image_view));
	}

	/* Create Frame Buffers */
	{
		for (int i = 0; i < _rsrs->no_of_swap_chain_images_; i++)
		{
			VkImageView attachments[] = { _rsrs->vk_swap_chain_image_views_[i], _rsrs->vk_depth_image_view };
			VkFramebufferCreateInfo framebuffer_info = { 0 };
			framebuffer_info = (VkFramebufferCreateInfo){
				.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.renderPass = _rsrs->vk_render_pass_,
				.attachmentCount = 2,
				.pAttachments = attachments,
				.width = _rsrs->vk_swap_chain_image_extent_2d_.width,
				.height = _rsrs->vk_swap_chain_image_extent_2d_.height,
				.layers = 1
			};

			if (vkCreateFramebuffer(vk_logical_device_, &framebuffer_info, NULL, &_rsrs->vk_swap_chain_frame_buffers_[i]) != VK_SUCCESS)
			{
				printf("Failed to create frame buffer\n");
			}
		}
	}
	vkDeviceWaitIdle(vk_logical_device_);



}
