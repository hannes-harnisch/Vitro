module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <memory>
export module vt.Graphics.Vulkan.RenderTarget;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Graphics.RenderTargetSpecification;
import vt.Graphics.SwapChain;
import vt.Graphics.Vulkan.Handle;

namespace vt::vulkan
{
	export class VulkanRenderTarget
	{
	public:
		VulkanRenderTarget(RenderTargetSpecification const& spec, DeviceApiTable const& api)
		{
			auto attachments = fill_attachments(spec);
			initialize_framebuffer(spec.render_pass, attachments, spec.width, spec.height, api);
		}

		VulkanRenderTarget(SharedRenderTargetSpecification const& spec,
						   SwapChain const&						  swap_chain,
						   unsigned								  back_buffer_index,
						   DeviceApiTable const&				  api)
		{
			auto attachments = fill_attachments(spec);

			auto swap_chain_img_dst	 = attachments.begin() + spec.shared_img_dst_index;
			auto swap_chain_img_view = swap_chain.vulkan.get_back_buffer_view(back_buffer_index);
			attachments.emplace(swap_chain_img_dst, swap_chain_img_view);

			initialize_framebuffer(spec.render_pass, attachments, swap_chain->get_width(), swap_chain->get_height(), api);
		}

		VkFramebuffer get_handle() const
		{
			return framebuffer.get();
		}

	private:
		UniqueVkFramebuffer framebuffer;

		static auto fill_attachments(auto const& spec)
		{
			FixedList<VkImageView, MAX_ATTACHMENTS> attachments;
			for(auto attachment : spec.color_attachments)
				attachments.emplace_back(attachment->vulkan.get_view());

			if(spec.depth_stencil_attachment)
				attachments.emplace_back(spec.depth_stencil_attachment->vulkan.get_view());

			return attachments;
		}

		void initialize_framebuffer(RenderPass const&							   render_pass,
									FixedList<VkImageView, MAX_ATTACHMENTS> const& attachments,
									unsigned									   width,
									unsigned									   height,
									DeviceApiTable const&						   api)
		{
			VkFramebufferCreateInfo const framebuffer_info {
				.sType			 = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.renderPass		 = render_pass.vulkan.get_handle(),
				.attachmentCount = count(attachments),
				.pAttachments	 = attachments.data(),
				.width			 = width,
				.height			 = height,
				.layers			 = 1,
			};
			auto result = api.vkCreateFramebuffer(api.device, &framebuffer_info, nullptr, std::out_ptr(framebuffer, api));
			VT_CHECK_RESULT(result, "Failed to create Vulkan framebuffer.");
		}
	};
}
