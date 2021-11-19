module;
#include "VitroCore/Macros.hpp"
#include "VulkanAPI.hpp"

#include <memory>
#include <vector>
export module vt.Graphics.Vulkan.RenderPass;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Core.LookupTable;
import vt.Core.SmallList;
import vt.Graphics.RenderPassSpecification;
import vt.Graphics.Vulkan.Handle;
import vt.Graphics.Vulkan.Image;

namespace vt::vulkan
{
	constexpr inline auto IMAGE_LOAD_OP_LOOKUP = [] {
		LookupTable<ImageLoadOp, VkAttachmentLoadOp> _;
		using enum ImageLoadOp;

		_[Load]	  = VK_ATTACHMENT_LOAD_OP_LOAD;
		_[Clear]  = VK_ATTACHMENT_LOAD_OP_CLEAR;
		_[Ignore] = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		return _;
	}();

	constexpr inline auto IMAGE_STORE_OP_LOOKUP = [] {
		LookupTable<ImageStoreOp, VkAttachmentStoreOp> _;
		using enum ImageStoreOp;

		_[Store]  = VK_ATTACHMENT_STORE_OP_STORE;
		_[Ignore] = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		return _;
	}();

	// Helper struct for initializing and representing the list of Vulkan attachment descriptions, which can't be larger than
	// the maximum number of attachments.
	struct AttachmentDescriptionList : FixedList<VkAttachmentDescription, MAX_ATTACHMENTS>
	{
		AttachmentDescriptionList(RenderPassSpecification const& spec) : FixedList(spec.attachments.size())
		{
			auto attachment_desc = begin();
			for(auto attachment : spec.attachments)
				*attachment_desc++ = VkAttachmentDescription {
					.format			= IMAGE_FORMAT_LOOKUP[attachment.format],
					.samples		= static_cast<VkSampleCountFlagBits>(attachment.sample_count),
					.loadOp			= IMAGE_LOAD_OP_LOOKUP[attachment.load_op],
					.storeOp		= IMAGE_STORE_OP_LOOKUP[attachment.store_op],
					.stencilLoadOp	= IMAGE_LOAD_OP_LOOKUP[spec.stencil_load_op],
					.stencilStoreOp = IMAGE_STORE_OP_LOOKUP[spec.stencil_store_op],
					.initialLayout	= IMAGE_LAYOUT_LOOKUP[attachment.initial_layout],
					.finalLayout	= IMAGE_LAYOUT_LOOKUP[attachment.final_layout],
				};
		}
	};

	export class VulkanRenderPass
	{
	public:
		VulkanRenderPass(RenderPassSpecification const& spec, DeviceApiTable const& api)
		{
			bool const uses_depth_stencil = spec.uses_depth_stencil_attachment();

			AttachmentDescriptionList		 attachments(spec);
			SmallList<VkSubpassDescription>	 subpasses(spec.subpasses.size());
			SmallList<VkAttachmentReference> refs(count_attachment_refs(spec)); // Pointers to elements in this must stay valid.
			SmallList<uint32_t>				 preserves(count_preserve_attachments(spec));
			SmallList<VkSubpassDependency>	 dependencies;
			assign_initial_external_dependencies(dependencies);

			auto subpass_desc = subpasses.begin();
			auto ref		  = refs.begin();
			auto preserve	  = preserves.begin();

			uint32_t subpass_index		  = 0;
			uint32_t attachment_ref_count = 0;
			uint32_t preserve_count		  = 0;
			for(auto& subpass : spec.subpasses)
			{
				struct AttachmentUse
				{
					bool for_input	= false;
					bool for_output = false;
				};
				FixedList<AttachmentUse, MAX_ATTACHMENTS> uses(spec.attachments.size());

				bool requires_self_dependency = false;
				for(uint8_t index : subpass.input_attachments)
					uses[index].for_input = true;
				for(uint8_t index : subpass.output_attachments)
				{
					uses[index].for_output = true;
					if(uses[index].for_input)
						requires_self_dependency = true;
				}

				uint32_t input_refs_begin = attachment_ref_count;
				for(uint8_t index : subpass.input_attachments)
				{
					VkImageLayout layout;
					if(uses[index].for_input && uses[index].for_output)
						layout = VK_IMAGE_LAYOUT_GENERAL;
					else if(uses_depth_stencil && index == spec.attachments.size() - 1)
						layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
					else
						layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

					*ref++ = VkAttachmentReference {index, layout};
					++attachment_ref_count;
				}

				bool	 subpass_has_depth_stencil_output = false;
				uint32_t output_refs_begin				  = attachment_ref_count;
				for(uint8_t index : subpass.output_attachments)
				{
					if(uses_depth_stencil && index == spec.attachments.size() - 1)
					{											 // Skip assigning a reference for the depth stencil attachment
						subpass_has_depth_stencil_output = true; // here, as this section must have color attachment references
						continue;								 // contiguously, since they are read that way by Vulkan.
					}
					VkImageLayout layout;
					if(uses[index].for_input && uses[index].for_output)
						layout = VK_IMAGE_LAYOUT_GENERAL;
					else
						layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

					*ref++ = VkAttachmentReference {index, layout};
					++attachment_ref_count;
				}

				VkAttachmentReference const* depth_stencil = nullptr;
				if(subpass_has_depth_stencil_output)
				{
					VkImageLayout layout;
					if(uses.back().for_input && uses.back().for_output)
						layout = VK_IMAGE_LAYOUT_GENERAL;
					else
						layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

					*ref = VkAttachmentReference {count(uses) - 1, layout};
					++attachment_ref_count;
					depth_stencil = &*ref++;
				}

				uint32_t preserve_count_begin = preserve_count;
				for(uint8_t index : subpass.preserve_attachments)
				{
					*preserve++ = index;
					++preserve_count;
				}

				*subpass_desc++ = VkSubpassDescription {
					.pipelineBindPoint		 = VK_PIPELINE_BIND_POINT_GRAPHICS,
					.inputAttachmentCount	 = output_refs_begin - input_refs_begin,
					.pInputAttachments		 = &refs[input_refs_begin],
					.colorAttachmentCount	 = attachment_ref_count - output_refs_begin - subpass_has_depth_stencil_output,
					.pColorAttachments		 = &refs[output_refs_begin],
					.pResolveAttachments	 = nullptr,
					.pDepthStencilAttachment = depth_stencil,
					.preserveAttachmentCount = preserve_count - preserve_count_begin,
					.pPreserveAttachments	 = preserves.data() + preserve_count_begin,
				};
				if(requires_self_dependency)
					dependencies.emplace_back(get_self_dependency(subpass_index));
				if(subpass_index) // Skip assigning dependency on the first iteration.
					dependencies.emplace_back(get_regular_dependency(subpass_index));
				++subpass_index;
			}
			dependencies.emplace_back(get_final_external_dependency(subpass_index));

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

		static size_t count_preserve_attachments(RenderPassSpecification const& spec)
		{
			size_t count = 0;

			for(auto& subpass : spec.subpasses)
				count += subpass.preserve_attachments.size();

			return count;
		}

		static void assign_initial_external_dependencies(SmallList<VkSubpassDependency>& dependencies)
		{
			dependencies.emplace_back(VkSubpassDependency {
				.srcSubpass		 = VK_SUBPASS_EXTERNAL,
				.dstSubpass		 = 0,
				.srcStageMask	 = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				.dstStageMask	 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.srcAccessMask	 = VK_ACCESS_MEMORY_READ_BIT,
				.dstAccessMask	 = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
			});
			dependencies.emplace_back(VkSubpassDependency {
				.srcSubpass		 = VK_SUBPASS_EXTERNAL,
				.dstSubpass		 = 0,
				.srcStageMask	 = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
				.dstStageMask	 = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
				.srcAccessMask	 = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				.dstAccessMask	 = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
			});
		}

		static VkSubpassDependency get_final_external_dependency(uint32_t subpass_index)
		{
			return {
				.srcSubpass		 = subpass_index - 1,
				.dstSubpass		 = VK_SUBPASS_EXTERNAL,
				.srcStageMask	 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.dstStageMask	 = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				.srcAccessMask	 = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				.dstAccessMask	 = VK_ACCESS_MEMORY_READ_BIT,
				.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
			};
		}

		static VkSubpassDependency get_self_dependency(uint32_t subpass_index)
		{
			return {
				.srcSubpass		 = subpass_index,
				.dstSubpass		 = subpass_index,
				.srcStageMask	 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.dstStageMask	 = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				.srcAccessMask	 = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				.dstAccessMask	 = VK_ACCESS_SHADER_READ_BIT,
				.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
			};
		}

		// TODO: Can this be refined to be more fine-grained towards how attachments are written and read between subpasses?
		static VkSubpassDependency get_regular_dependency(uint32_t subpass_index)
		{
			return {
				.srcSubpass		 = subpass_index - 1,
				.dstSubpass		 = subpass_index,
				.srcStageMask	 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.dstStageMask	 = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				.srcAccessMask	 = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				.dstAccessMask	 = VK_ACCESS_SHADER_READ_BIT,
				.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
			};
		}
	};
}
