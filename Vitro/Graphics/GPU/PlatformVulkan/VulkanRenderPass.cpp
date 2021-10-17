module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"
export module vt.Graphics.Vulkan.RenderPass;

import vt.Core.FixedList;
import vt.Graphics.RenderPassSpecification;
import vt.Graphics.Vulkan.Driver;
import vt.Graphics.Vulkan.Texture;

namespace vt::vulkan
{
	VkAttachmentLoadOp convert_image_load_op(ImageLoadOp op)
	{
		using enum ImageLoadOp;
		switch(op)
		{ // clang-format off
			case Load:   return VK_ATTACHMENT_LOAD_OP_LOAD;
			case Clear:  return VK_ATTACHMENT_LOAD_OP_CLEAR;
			case Ignore: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		}
		VT_UNREACHABLE();
	}

	VkAttachmentStoreOp convert_image_store_op(ImageStoreOp op)
	{
		switch(op)
		{
			case ImageStoreOp::Store:  return VK_ATTACHMENT_STORE_OP_STORE;
			case ImageStoreOp::Ignore: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}
		VT_UNREACHABLE();
	}

	export VkSampleCountFlagBits convert_sample_count(uint8_t count)
	{
		switch(count)
		{
			case 1:	 return VK_SAMPLE_COUNT_1_BIT;
			case 2:	 return VK_SAMPLE_COUNT_2_BIT;
			case 4:	 return VK_SAMPLE_COUNT_4_BIT;
			case 8:	 return VK_SAMPLE_COUNT_8_BIT;
			case 16: return VK_SAMPLE_COUNT_16_BIT;
			case 32: return VK_SAMPLE_COUNT_32_BIT;
			case 64: return VK_SAMPLE_COUNT_64_BIT;
		} // clang-format on
		VT_UNREACHABLE();
	}

	export class VulkanRenderPass
	{
	public:
		VulkanRenderPass(RenderPassSpecification const& spec, DeviceApiTable const& api)
		{
			FixedList<VkAttachmentDescription, MAX_ATTACHMENTS> attachments;
			for(auto attachment : spec.attachments)
				attachments.emplace_back(VkAttachmentDescription {
					.format			= convert_image_format(attachment.format),
					.samples		= convert_sample_count(attachment.sample_count),
					.loadOp			= convert_image_load_op(attachment.load_op),
					.storeOp		= convert_image_store_op(attachment.store_op),
					.stencilLoadOp	= convert_image_load_op(spec.stencil_load_op),
					.stencilStoreOp = convert_image_store_op(spec.stencil_store_op),
					.initialLayout	= convert_image_layout(attachment.initial_layout),
					.finalLayout	= convert_image_layout(attachment.final_layout),
				});

			std::vector<VkSubpassDescription>  subpasses;
			std::vector<VkSubpassDependency>   dependencies;
			std::vector<VkAttachmentReference> refs;
			subpasses.reserve(spec.subpasses.size());
			dependencies.reserve(spec.subpasses.size() + 1);
			for(auto& subpass : spec.subpasses)
			{
				subpasses.emplace_back(VkSubpassDescription {
					.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
				});
			}

			VkRenderPassCreateInfo const render_pass_info {
				.sType			 = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
				.attachmentCount = count(attachments),
				.pAttachments	 = attachments.data(),
				.subpassCount	 = count(subpasses),
				.pSubpasses		 = subpasses.data(),
				.dependencyCount = count(dependencies),
				.pDependencies	 = dependencies.data(),
			};
			VkRenderPass unowned_render_pass;

			auto result = api.vkCreateRenderPass(api.device, &render_pass_info, nullptr, &unowned_render_pass);
			render_pass.reset(unowned_render_pass, api);
			VT_ENSURE_RESULT(result, "Failed to create Vulkan render pass.");
		}

		VkRenderPass ptr() const
		{
			return render_pass.get();
		}

	private:
		UniqueVkRenderPass render_pass;
	};
}
