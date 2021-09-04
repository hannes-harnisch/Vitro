module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <optional>
#include <ranges>
export module vt.D3D12.RenderPass;

import vt.Core.FixedList;
import vt.D3D12.Texture;
import vt.Graphics.Device;
import vt.Graphics.RenderPassInfo;

namespace vt::d3d12
{
	D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE convert_image_load_op(ImageLoadOp op)
	{
		switch(op)
		{
			case ImageLoadOp::Load: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
			case ImageLoadOp::Clear: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
			case ImageLoadOp::Ignore: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
		}
		VT_UNREACHABLE();
	}

	D3D12_RENDER_PASS_ENDING_ACCESS_TYPE convert_image_store_op(ImageStoreOp op)
	{
		switch(op)
		{
			case ImageStoreOp::Store: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
			case ImageStoreOp::Ignore: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
		}
		VT_UNREACHABLE();
	}

	struct AttachmentInfo
	{
		DXGI_FORMAT								format;
		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE begin_access;
		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE	end_access;

		AttachmentInfo(vt::AttachmentInfo info) :
			format(convert_image_format(info.format)),
			begin_access(convert_image_load_op(info.load_op)),
			end_access(convert_image_store_op(info.store_op))
		{}
	};

	export struct AttachmentTransition
	{
		unsigned			  index;
		D3D12_RESOURCE_STATES old_layout;
		D3D12_RESOURCE_STATES new_layout;
	};

	export using TransitionList = FixedList<AttachmentTransition, MaxAttachments>;

	export class RenderPass
	{
	public:
		FixedList<AttachmentInfo, MaxAttachments> attachments;
		bool									  uses_depth_stencil;
		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE	  stencil_begin_access;
		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE	  stencil_end_access;
		FixedList<TransitionList, MaxSubpasses>	  subpasses;
		TransitionList							  final_transitions;

		RenderPass(vt::Device const&, RenderPassInfo const& info) :
			attachments(info.attachments.begin(), info.attachments.end()),
			uses_depth_stencil(contains_depth_stencil_attachment(info.attachments)),
			stencil_begin_access(convert_image_load_op(info.stencil_load_op)),
			stencil_end_access(convert_image_store_op(info.stencil_store_op))
		{
			FixedList<D3D12_RESOURCE_STATES, MaxAttachments> prev_layouts;
			for(auto attachment : info.attachments)
				prev_layouts.emplace_back(convert_image_layout(attachment.initial_layout));

			for(auto& subpass : info.subpasses)
			{
				TransitionList transitions;
				for(auto ref : subpass.output_refs)
				{
					auto old_layout = prev_layouts[ref.index];
					auto new_layout = convert_image_layout(ref.used_layout);
					if(old_layout == new_layout)
						continue;

					transitions.emplace_back(ref.index, old_layout, new_layout);
					prev_layouts[ref.index] = new_layout;
				}
				subpasses.emplace_back(transitions);
			}

			for(unsigned i = 0; i != info.attachments.size(); ++i)
			{
				auto prev  = prev_layouts[i];
				auto final = convert_image_layout(info.attachments[i].final_layout);
				if(prev == final)
					continue;

				final_transitions.emplace_back(i, prev, final);
			}
		}

	private:
		static bool contains_depth_stencil_attachment(auto& attachments)
		{
			auto uses_depth_stencil_layout = [](auto attachment) {
				return attachment.final_layout == ImageLayout::DepthStencilAttachment ||
					   attachment.final_layout == ImageLayout::DepthStencilReadOnly;
			};
			auto it = std::find_if(attachments.begin(), attachments.end(), uses_depth_stencil_layout);

			auto second = std::find_if(it, attachments.end(), uses_depth_stencil_layout);
			VT_ASSERT(second == attachments.end(), "Multiple depth stencil attachments are unsupported.");

			return it != attachments.end();
		}
	};
}
