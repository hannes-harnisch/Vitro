module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <memory>
#include <vector>
export module vt.Graphics.Vulkan.RenderPass;

import vt.Core.FixedList;
import vt.Core.SmallList;
import vt.Graphics.RenderPassSpecification;
import vt.Graphics.Vulkan.Handle;
import vt.Graphics.Vulkan.Image;

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
			bool const uses_depth_stencil = spec.uses_depth_stencil_attachment();

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

			SmallList<VkSubpassDescription> subpasses;
			subpasses.reserve(spec.subpasses.size());
			SmallList<VkAttachmentReference> refs;
			refs.reserve(count_attachment_refs(spec)); // This reserve is crucial, because pointers to VkAttachmentReferences
													   // must remain valid throughout the whole preparation of the create info.
			SmallList<VkSubpassDependency> dependencies;
			dependencies.reserve(spec.subpasses.size() + 1);
			dependencies.emplace_back(VkSubpassDependency {
				.srcSubpass		 = VK_SUBPASS_EXTERNAL,
				.dstSubpass		 = 0,
				.srcStageMask	 = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				.dstStageMask	 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.srcAccessMask	 = VK_ACCESS_MEMORY_READ_BIT,
				.dstAccessMask	 = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
			});
			uint32_t subpass_index = 0;
			for(auto& subpass : spec.subpasses)
			{
				struct AttachmentUse
				{
					bool for_input	= false;
					bool for_output = false;
				};
				FixedList<AttachmentUse, 2 * MAX_ATTACHMENTS> uses(subpass.input_attachments.size() +
																   subpass.output_attachments.size());

				bool requires_self_dependency = false;
				for(uint8_t index : subpass.input_attachments)
					uses[index].for_input = true;
				for(uint8_t index : subpass.output_attachments)
				{
					uses[index].for_output = true;
					if(uses[index].for_input)
						requires_self_dependency = true;
				}

				uint32_t input_refs_begin = count(refs);
				for(uint8_t index : subpass.input_attachments)
				{
					VkImageLayout layout;
					if(uses[index].for_input && uses[index].for_output)
						layout = VK_IMAGE_LAYOUT_GENERAL;
					else if(uses_depth_stencil && index == spec.attachments.size() - 1)
						layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
					else
						layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

					refs.emplace_back(index, layout);
				}

				bool	 subpass_has_depth_stencil_output = false;
				uint32_t output_refs_begin				  = count(refs);
				for(uint8_t index : subpass.output_attachments)
				{
					if(uses_depth_stencil && index == spec.attachments.size() - 1)
					{
						subpass_has_depth_stencil_output = true;
						continue;
					}
					VkImageLayout layout;
					if(uses[index].for_input && uses[index].for_output)
						layout = VK_IMAGE_LAYOUT_GENERAL;
					else
						layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

					refs.emplace_back(index, layout);
				}

				VkAttachmentReference const* depth_stencil = nullptr;
				if(subpass_has_depth_stencil_output)
				{
					VkImageLayout layout;
					if(uses.back().for_input && uses.back().for_output)
						layout = VK_IMAGE_LAYOUT_GENERAL;
					else
						layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

					depth_stencil = &refs.emplace_back(count(uses) - 1, layout);
				}

				subpasses.emplace_back(VkSubpassDescription {
					.pipelineBindPoint		 = VK_PIPELINE_BIND_POINT_GRAPHICS,
					.inputAttachmentCount	 = output_refs_begin - input_refs_begin,
					.pInputAttachments		 = &refs[input_refs_begin],
					.colorAttachmentCount	 = count(refs) - output_refs_begin - subpass_has_depth_stencil_output,
					.pColorAttachments		 = &refs[output_refs_begin],
					.pResolveAttachments	 = nullptr,
					.pDepthStencilAttachment = depth_stencil,
					.preserveAttachmentCount = 0,
					.pPreserveAttachments	 = nullptr,
				});

				if(requires_self_dependency)
					dependencies.emplace_back(VkSubpassDependency {
						.srcSubpass		 = subpass_index,
						.dstSubpass		 = subpass_index,
						.srcStageMask	 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
						.dstStageMask	 = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
						.srcAccessMask	 = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
						.dstAccessMask	 = VK_ACCESS_SHADER_READ_BIT,
						.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
					});

				if(subpass_index) // Skip adding dependency on the first iteration.
					dependencies.emplace_back(VkSubpassDependency {
						.srcSubpass		 = subpass_index - 1,
						.dstSubpass		 = subpass_index,
						.srcStageMask	 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
						.dstStageMask	 = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
						.srcAccessMask	 = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
						.dstAccessMask	 = VK_ACCESS_SHADER_READ_BIT,
						.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
					});
				++subpass_index;
			}

			dependencies.emplace_back(VkSubpassDependency {
				.srcSubpass		 = subpass_index - 1,
				.dstSubpass		 = VK_SUBPASS_EXTERNAL,
				.srcStageMask	 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.dstStageMask	 = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				.srcAccessMask	 = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				.dstAccessMask	 = VK_ACCESS_MEMORY_READ_BIT,
				.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
			});
			VkRenderPassCreateInfo const render_pass_info {
				.sType			 = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
				.attachmentCount = count(attachments),
				.pAttachments	 = attachments.data(),
				.subpassCount	 = count(subpasses),
				.pSubpasses		 = subpasses.data(),
				.dependencyCount = count(dependencies),
				.pDependencies	 = dependencies.data(),
			};
			auto result = api.vkCreateRenderPass(api.device, &render_pass_info, nullptr, std::out_ptr(render_pass, api));
			VT_CHECK_RESULT(result, "Failed to create Vulkan render pass.");
		}

		VkRenderPass get_handle() const
		{
			return render_pass.get();
		}

	private:
		UniqueVkRenderPass render_pass;

		static size_t count_attachment_refs(RenderPassSpecification const& spec)
		{
			size_t count = 0;

			for(auto& subpass : spec.subpasses)
				count += subpass.input_attachments.size() + subpass.output_attachments.size();

			return count;
		}
	};
}
