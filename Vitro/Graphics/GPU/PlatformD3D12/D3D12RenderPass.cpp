﻿module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <algorithm>
#include <vector>
export module vt.Graphics.D3D12.RenderPass;

import vt.Core.Array;
import vt.Core.Enum;
import vt.Core.FixedList;
import vt.Graphics.D3D12.Texture;
import vt.Graphics.RenderPassSpecification;

namespace vt::d3d12
{
	D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE convert_image_load_op(ImageLoadOp op)
	{
		using enum ImageLoadOp;
		switch(op)
		{ // clang-format off
			case Load:   return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
			case Clear:  return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
			case Ignore: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
		}
		VT_UNREACHABLE();
	}

	D3D12_RENDER_PASS_ENDING_ACCESS_TYPE convert_image_store_op(ImageStoreOp op)
	{
		switch(op)
		{
			case ImageStoreOp::Store:  return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
			case ImageStoreOp::Ignore: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
		} // clang-format on
		VT_UNREACHABLE();
	}

	struct AttachmentAccess
	{
		DXGI_FORMAT								format : 8; // TODO: Replace with enum reflection
		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE begin_access : 3;
		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE	end_access : 3;

		AttachmentAccess(AttachmentSpecification spec) :
			format(convert_image_format(spec.format)),
			begin_access(convert_image_load_op(spec.load_op)),
			end_access(convert_image_store_op(spec.store_op))
		{}
	};

	export struct AttachmentTransition
	{
		unsigned			  index;
		D3D12_RESOURCE_STATES old_layout;
		D3D12_RESOURCE_STATES new_layout;
	};

	export using TransitionList = FixedList<AttachmentTransition, MAX_ATTACHMENTS>;

	export class D3D12RenderPass
	{
	public:
		D3D12RenderPass(RenderPassSpecification const& spec) :
			attachments(spec.attachments.begin(), spec.attachments.end()),
			is_using_depth_stencil(contains_depth_stencil_attachment(spec.attachments)),
			stencil_begin_access(convert_image_load_op(spec.stencil_load_op)),
			stencil_end_access(convert_image_store_op(spec.stencil_store_op))
		{
			FixedList<D3D12_RESOURCE_STATES, MAX_ATTACHMENTS> prev_layouts;
			for(auto attachment : spec.attachments)
				prev_layouts.emplace_back(convert_image_layout(attachment.initial_layout));

			for(auto& subpass : spec.subpasses)
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
				subpass_transitions.emplace_back(transitions);
			}

			for(unsigned i = 0; i != spec.attachments.size(); ++i)
			{
				auto prev  = prev_layouts[i];
				auto final = convert_image_layout(spec.attachments[i].final_layout);
				if(prev == final)
					continue;

				final_transitions.emplace_back(i, prev, final);
			}
		}

		ArrayView<AttachmentAccess> get_render_target_attachments() const
		{
			return {attachments.begin(), attachments.end() - is_using_depth_stencil};
		}

		DXGI_FORMAT get_render_target_format(size_t index) const
		{
			return attachments[index].format;
		}

		bool uses_depth_stencil() const
		{
			return is_using_depth_stencil;
		}

		DXGI_FORMAT get_depth_stencil_format() const
		{
			return attachments.back().format;
		}

		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE get_depth_begin_access() const
		{
			return attachments.back().begin_access;
		}

		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE get_depth_end_access() const
		{
			return attachments.back().end_access;
		}

		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE get_stencil_begin_access() const
		{
			return stencil_begin_access;
		}

		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE get_stencil_end_access() const
		{
			return stencil_end_access;
		}

		TransitionList const& get_subpass_transitions(size_t index) const
		{
			return subpass_transitions[index];
		}

		TransitionList const& get_final_transitions() const
		{
			return final_transitions;
		}

		size_t subpass_count() const
		{
			return subpass_transitions.size();
		}

	private:
		FixedList<AttachmentAccess, MAX_ATTACHMENTS> attachments;
		bool										 is_using_depth_stencil;
		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE		 stencil_begin_access;
		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE		 stencil_end_access;
		std::vector<TransitionList>					 subpass_transitions;
		TransitionList								 final_transitions;

		static bool contains_depth_stencil_attachment(RenderPassSpecification::AttachmentList const& attachments)
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
