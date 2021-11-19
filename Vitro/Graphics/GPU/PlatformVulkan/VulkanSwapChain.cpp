module;
#include "VitroCore/Macros.hpp"
#include "VulkanAPI.hpp"

#include <algorithm>
#include <memory>
#include <optional>
export module vt.Graphics.Vulkan.SwapChain;

import vt.App.AppContextBase;
import vt.App.Window;
import vt.Core.Algorithm;
import vt.Core.Array;
import vt.Core.Rect;
import vt.Core.SmallList;
import vt.Graphics.AbstractSwapChain;
import vt.Graphics.Vulkan.Handle;
import vt.Graphics.Vulkan.SyncTokenPool;
import vt.Graphics.Vulkan.Image;

bool operator==(VkSurfaceFormatKHR const& left, VkSurfaceFormatKHR const& right)
{
	return left.format == right.format && left.colorSpace == right.colorSpace;
}

namespace vt::vulkan
{
	struct SurfaceDeleter
	{
		using pointer = VkSurfaceKHR;
		void operator()(VkSurfaceKHR surface) const
		{
			auto& driver = InstanceApiTable::get();
			driver.vkDestroySurfaceKHR(driver.instance, surface, nullptr);
		}
	};
	using UniqueVkSurfaceKHR = std::unique_ptr<VkSurfaceKHR, SurfaceDeleter>;

	UniqueVkSurfaceKHR make_surface(Window& window)
	{
#if VT_SYSTEM_WINDOWS

		VkWin32SurfaceCreateInfoKHR const surface_info {
			.sType	   = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			.hinstance = AppContextBase::get_system_window_owner(),
			.hwnd	   = window.native_handle(),
		};
		auto create_surface = &InstanceApiTable::vkCreateWin32SurfaceKHR;

#elif VT_SYSTEM_LINUX

		VkXcbSurfaceCreateInfoKHR const surface_info {
			.sType		= VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
			.connection = AppContextBase::get_system_window_owner(),
			.window		= window.native_handle(),
		};
		auto create_surface = &InstanceApiTable::vkCreateXcbSurfaceKHR;

#endif

		UniqueVkSurfaceKHR surface;

		auto& driver = InstanceApiTable::get();
		auto  result = (driver.*create_surface)(driver.instance, &surface_info, nullptr, std::out_ptr(surface));
		VT_CHECK_RESULT(result, "Failed to create Vulkan surface.");

		return surface;
	}

	export class VulkanSwapChain final : public AbstractSwapChain
	{
	public:
		VulkanSwapChain(uint32_t			  render_queue_family_index,
						Window&				  window,
						uint8_t				  buffer_count,
						SyncTokenPool&		  sync_token_pool,
						DeviceApiTable const& api) :
			api(&api), sync_token_pool(&sync_token_pool), surface(make_surface(window))
		{
			check_queue_support(render_queue_family_index);
			initialize_surface_format();
			check_vsync_support();
			initialize_swapchain(buffer_count, &window, {});
		}

		ImageFormat get_format() const override
		{
			return image_format;
		}

		unsigned get_current_image_index() const override
		{
			return current_img_index;
		}

		unsigned count_presents() const override
		{
			return present_count;
		}

		unsigned count_buffers() const override
		{
			return count(images);
		}

		Extent get_size() const override
		{
			return buffer_size;
		}

		void resize(Extent size) override
		{
			uint32_t img_count = count(images);

			images.clear();
			image_views.clear();

			initialize_swapchain(img_count, nullptr, size);
		}

		void enable_vsync() override
		{
			present_mode = mailbox_supported ? VK_PRESENT_MODE_MAILBOX_KHR : VK_PRESENT_MODE_FIFO_KHR;
			resize(buffer_size);
		}

		void disable_vsync() override
		{
			present_mode = tearing_supported ? VK_PRESENT_MODE_IMMEDIATE_KHR : VK_PRESENT_MODE_FIFO_KHR;
			resize(buffer_size);
		}

		std::optional<SyncToken> request_frame() override
		{
			if(should_be_resized)
				return {};

			auto token = sync_token_pool->acquire_token(*api);

			uint32_t image_index;
			auto	 result = api->vkAcquireNextImageKHR(api->device, swapchain.get(), 0, token.vulkan.semaphore,
													 token.vulkan.fence, &image_index);

			if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				should_be_resized = true;
				return {};
			}

			VT_CHECK_RESULT(result, "Failed to acquire Vulkan swap chain image.");
			current_img_index = static_cast<uint8_t>(image_index);
			return token;
		}

		void present(VkSemaphore wait_semaphore, VkQueue render_queue)
		{
			VT_ASSERT(!should_be_resized, "This method should not be called if request_frame has failed earlier.");

			auto	 swapchain_handle = swapchain.get();
			uint32_t image_index	  = current_img_index;

			VkPresentInfoKHR const present_info {
				.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
				.waitSemaphoreCount = 1,
				.pWaitSemaphores	= &wait_semaphore,
				.swapchainCount		= 1,
				.pSwapchains		= &swapchain_handle,
				.pImageIndices		= &image_index,
				.pResults			= nullptr,
			};
			auto result = api->vkQueuePresentKHR(render_queue, &present_info);

			if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
				should_be_resized = true;
			else
			{
				VT_CHECK_RESULT(result, "Failed to present with Vulkan swap chain.");
				++present_count;
			}
		}

		VkImageView get_back_buffer_view(size_t index) const
		{
			return image_views[index].get();
		}

	private:
		DeviceApiTable const*		 api;
		SyncTokenPool*				 sync_token_pool;
		Extent						 buffer_size;
		UniqueVkSurfaceKHR			 surface;
		VkSurfaceFormatKHR			 surface_format;
		ImageFormat					 image_format;
		uint8_t						 current_img_index;
		bool						 tearing_supported = false;
		bool						 mailbox_supported = false;
		VkPresentModeKHR			 present_mode	   = VK_PRESENT_MODE_FIFO_KHR;
		unsigned					 present_count	   = 0;
		bool						 should_be_resized;
		UniqueVkSwapchainKHR		 swapchain;
		SmallList<VkImage>			 images;
		SmallList<UniqueVkImageView> image_views;

		static VkExtent2D clamp_extent(Extent size, VkSurfaceCapabilitiesKHR const& capabilities)
		{
			return {std::clamp(size.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
					std::clamp(size.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)};
		}

		void check_queue_support(uint32_t render_queue_family_index) const
		{
			auto& driver = InstanceApiTable::get();

			VkBool32 supported;
			auto result = driver.vkGetPhysicalDeviceSurfaceSupportKHR(api->adapter, render_queue_family_index, surface.get(),
																	  &supported);
			VT_CHECK_RESULT(result, "Failed to query Vulkan queue support for a surface object.");
			VT_ENSURE(supported, "A window was created that Vulkan cannot present to.");
		}

		void initialize_surface_format()
		{
			auto& driver = InstanceApiTable::get();

			uint32_t count;
			auto	 result = driver.vkGetPhysicalDeviceSurfaceFormatsKHR(api->adapter, surface.get(), &count, nullptr);
			VT_CHECK_RESULT(result, "Failed to query Vulkan surface capability count.");

			SmallList<VkSurfaceFormatKHR> surface_formats(count);
			result = driver.vkGetPhysicalDeviceSurfaceFormatsKHR(api->adapter, surface.get(), &count, surface_formats.data());
			VT_CHECK_RESULT(result, "Failed to query Vulkan surface capabilities.");

			constexpr VkSurfaceFormatKHR preferred_format {
				.format		= VK_FORMAT_R8G8B8A8_UNORM,
				.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
			};

			surface_format = surface_formats[0];
			if(contains(surface_formats, preferred_format))
				surface_format = preferred_format;

			image_format = VULKAN_FORMAT_LOOKUP[surface_format.format];
		}

		void check_vsync_support()
		{
			auto& driver = InstanceApiTable::get();

			uint32_t count;
			auto	 result = driver.vkGetPhysicalDeviceSurfacePresentModesKHR(api->adapter, surface.get(), &count, nullptr);
			VT_CHECK_RESULT(result, "Failed to query Vulkan surface present mode count.");

			SmallList<VkPresentModeKHR> modes(count);
			result = driver.vkGetPhysicalDeviceSurfacePresentModesKHR(api->adapter, surface.get(), &count, modes.data());
			VT_CHECK_RESULT(result, "Failed to query Vulkan surface present modes.");

			if(contains(modes, VK_PRESENT_MODE_IMMEDIATE_KHR))
			{
				tearing_supported = true;
				present_mode	  = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}

			if(contains(modes, VK_PRESENT_MODE_MAILBOX_KHR))
				mailbox_supported = true;
		}

		void initialize_swapchain(uint32_t img_count, Window* window, Extent size)
		{
			auto& driver = InstanceApiTable::get();

			VkSurfaceCapabilitiesKHR capabilities;
			auto result = driver.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(api->adapter, surface.get(), &capabilities);
			VT_CHECK_RESULT(result, "Failed to query Vulkan surface capabilities.");

			VkExtent2D extent;
			if(capabilities.currentExtent.width != UINT32_MAX)
				extent = capabilities.currentExtent;
			else if(window)
				extent = clamp_extent(window->get_size(), capabilities);
			else
				extent = clamp_extent(size, capabilities);

			buffer_size = {extent.width, extent.height};

			VkSwapchainCreateInfoKHR const swapchain_info {
				.sType				   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
				.surface			   = surface.get(),
				.minImageCount		   = img_count,
				.imageFormat		   = surface_format.format,
				.imageColorSpace	   = surface_format.colorSpace,
				.imageExtent		   = extent,
				.imageArrayLayers	   = 1,
				.imageUsage			   = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				.imageSharingMode	   = VK_SHARING_MODE_EXCLUSIVE,
				.queueFamilyIndexCount = 0,
				.pQueueFamilyIndices   = nullptr,
				.preTransform		   = capabilities.currentTransform,
				.compositeAlpha		   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
				.presentMode		   = present_mode,
				.clipped			   = true,
				.oldSwapchain		   = swapchain.get(),
			};
			result = api->vkCreateSwapchainKHR(api->device, &swapchain_info, nullptr, std::out_ptr(swapchain, *api));
			VT_CHECK_RESULT(result, "Failed to create Vulkan swap chain.");

			should_be_resized = false;
			initialize_images(img_count);
		}

		void initialize_images(uint32_t img_count)
		{
			auto result = api->vkGetSwapchainImagesKHR(api->device, swapchain.get(), &img_count, nullptr);
			VT_CHECK_RESULT(result, "Failed to get Vulkan swap chain image count.");

			images.resize(img_count);
			result = api->vkGetSwapchainImagesKHR(api->device, swapchain.get(), &img_count, images.data());
			VT_CHECK_RESULT(result, "Failed to get Vulkan swap chain images.");

			for(auto image : images)
			{
				VkImageViewCreateInfo const image_view_info {
					.sType	  = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
					.image	  = image,
					.viewType = VK_IMAGE_VIEW_TYPE_2D,
					.format	  = surface_format.format,
					.components {
						.r = VK_COMPONENT_SWIZZLE_IDENTITY,
						.g = VK_COMPONENT_SWIZZLE_IDENTITY,
						.b = VK_COMPONENT_SWIZZLE_IDENTITY,
						.a = VK_COMPONENT_SWIZZLE_IDENTITY,
					},
					.subresourceRange {
						.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT,
						.baseMipLevel	= 0,
						.levelCount		= 1,
						.baseArrayLayer = 0,
						.layerCount		= 1,
					},
				};
				auto& image_view = image_views.emplace_back();
				result = api->vkCreateImageView(api->device, &image_view_info, nullptr, std::out_ptr(image_view, *api));
				VT_CHECK_RESULT(result, "Failed to create Vulkan image views for swap chain.");
			}
		}
	};
}
