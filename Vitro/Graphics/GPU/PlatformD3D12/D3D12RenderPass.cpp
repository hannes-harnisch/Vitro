module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

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
		DXGI_FORMAT								format		 : 8; // TODO: Replace with enum reflection
		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE begin_access : 3;
		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE	end_access	 : 3;

		AttachmentAccess(AttachmentSpecification spec) :
			format(convert_image_format(spec.format)),
			begin_access(convert_image_load_op(spec.load_op)),
			end_access(convert_image_store_op(spec.store_op))
		{}
	};

	struct AttachmentTransition
	{
		unsigned			  index;
		D3D12_RESOURCE_STATES old_layout;
		D3D12_RESOURCE_STATES new_layout;
	};

	export using TransitionList = FixedList<AttachmentTransition, MAX_ATTACHMENTS>;
	struct Subpass
	{
		D3D12_RENDER_PASS_FLAGS flags;
		TransitionList			transitions;
	};

	export class D3D12RenderPass
	{
	public:
		D3D12RenderPass(RenderPassSpecification const& spec) :
			attachments(spec.attachments.begin(), spec.attachments.end()),
			is_using_depth_stencil(spec.uses_depth_stencil_attachment()),
			stencil_begin_access(convert_image_load_op(spec.stencil_load_op)),
			stencil_end_access(convert_image_store_op(spec.stencil_store_op))
		{
			FixedList<D3D12_RESOURCE_STATES, MAX_ATTACHMENTS> current_layouts;
			for(auto attachment : spec.attachments)
				current_layouts.emplace_back(convert_image_layout(attachment.initial_layout));

			for(auto& subpass : spec.subpasses)
			{
				struct AttachmentUse
				{
					bool for_input	= false;
					bool for_output = false;
				};
				FixedList<AttachmentUse, 2 * MAX_ATTACHMENTS> uses(subpass.input_attachments.size() +
																   subpass.output_attachments.size());
				for(uint8_t index : subpass.input_attachments)
					uses[index].for_input = true;
				for(uint8_t index : subpass.output_attachments)
					uses[index].for_output = true;

				D3D12_RENDER_PASS_FLAGS flags = D3D12_RENDER_PASS_FLAG_NONE;

				TransitionList transitions;

				unsigned index = 0;
				for(auto [for_input, for_output] : uses)
				{
					D3D12_RESOURCE_STATES new_depth_layout, new_color_layout;
					if(for_input && for_output)
					{
						flags |= D3D12_RENDER_PASS_FLAG_ALLOW_UAV_WRITES;
						new_depth_layout = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
						new_color_layout = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
					}
					else if(for_output)
					{
						new_depth_layout = D3D12_RESOURCE_STATE_DEPTH_WRITE;
						new_color_layout = D3D12_RESOURCE_STATE_RENDER_TARGET;
					}
					else
					{
						new_depth_layout = D3D12_RESOURCE_STATE_DEPTH_READ;
						new_color_layout = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE |
										   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
					}
					auto old_layout = current_layouts[index];
					auto new_layout = uses_depth_stencil() && index == spec.attachments.size() - 1 ? new_depth_layout
																								   : new_color_layout;
					if(old_layout == new_layout)
						continue;

					transitions.emplace_back(index, old_layout, new_layout);
					current_layouts[index] = new_layout;
					++index;
				}
				subpasses.emplace_back(flags, transitions);
			}

			for(int i = 0; i != spec.attachments.size(); ++i)
			{
				auto prev  = current_layouts[i];
				auto final = convert_image_layout(spec.attachments[i].final_layout);
				if(prev == final)
					continue;

				final_transitions.emplace_back(i, prev, final);
			}
		}

		ArrayView<AttachmentAccess> get_render_target_accesses() const
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

		Subpass const& get_subpass(size_t index) const
		{
			return subpasses[index];
		}

		TransitionList const& get_final_transitions() const
		{
			return final_transitions;
		}

		size_t subpass_count() const
		{
			return subpasses.size();
		}

	private:
		FixedList<AttachmentAccess, MAX_ATTACHMENTS> attachments;
		bool										 is_using_depth_stencil;
		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE		 stencil_begin_access;
		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE		 stencil_end_access;
		std::vector<Subpass>						 subpasses;
		TransitionList								 final_transitions;
	};
}
