module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <vector>
export module vt.Graphics.D3D12.RenderPass;

import vt.Core.Array;
import vt.Core.Enum;
import vt.Core.FixedList;
import vt.Core.SmallList;
import vt.Graphics.D3D12.Image;
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

	struct AttachmentTransition
	{
		uint8_t				  index;
		bool				  requires_uav_barrier;
		D3D12_RESOURCE_STATES old_layout;
		D3D12_RESOURCE_STATES new_layout;
	};
	export using TransitionList = FixedList<AttachmentTransition, MAX_ATTACHMENTS>;

	// Stores the access types of a specific color or depth stencil attachment referenced in a subpass.
	struct SubpassAccess
	{
		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE begin : 4;
		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE	end	  : 4;
		bool									for_input; // Indicates whether the attachment is used as an input attachment.
	};
	using AccessList = FixedList<SubpassAccess, MAX_ATTACHMENTS>;

	// Stores all information related to a specific subpass of a D3D12 render pass.
	struct Subpass
	{
		D3D12_RENDER_PASS_FLAGS					flags;
		TransitionList							transitions;
		AccessList								accesses;
		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE stencil_begin = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS;
		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE	stencil_end	  = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS;

		bool uses_depth_stencil() const
		{
			return stencil_begin != D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS;
		}

		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE get_depth_begin_access() const
		{
			VT_ASSERT(uses_depth_stencil(), "This method is invalid unless a subpass uses the depth stencil attachment.");
			return accesses.back().begin;
		}

		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE get_depth_end_access() const
		{
			VT_ASSERT(uses_depth_stencil(), "This method is invalid unless a subpass uses the depth stencil attachment.");
			return accesses.back().end;
		}
	};

	// Used only while creating a D3D12 render pass. Tracks what state a resource is expected to be in when a subpass begins.
	struct ResourceStateList : FixedList<D3D12_RESOURCE_STATES, MAX_ATTACHMENTS>
	{
		ResourceStateList(RenderPassSpecification const& spec) :
			FixedList<D3D12_RESOURCE_STATES, MAX_ATTACHMENTS>(spec.attachments.size())
		{
			auto current_layout = begin();
			for(auto attachment : spec.attachments)
				*current_layout++ = convert_image_layout(attachment.initial_layout);
		}
	};

	// Used only while creating a D3D12 render pass. Tracks what each subpass of a render pass uses an attachment for and in
	// which subpass they are used first and last respectively.
	struct RenderPassAttachmentUsage
	{
		struct AttachmentUsage
		{
			bool for_input	= false;
			bool for_output = false;
			bool preserve	= false;
		};
		using SubpassAttachmentUsageList = FixedList<AttachmentUsage, MAX_ATTACHMENTS>;
		using SubpassIndexList			 = FixedList<uint8_t, MAX_ATTACHMENTS>;

		SmallList<SubpassAttachmentUsageList> subpass_attachment_usage_lists;
		SubpassIndexList					  first_subpass_where_used;
		SubpassIndexList					  last_subpass_where_used;

		RenderPassAttachmentUsage(RenderPassSpecification const& spec) :
			subpass_attachment_usage_lists(spec.subpasses.size()),
			first_subpass_where_used(spec.attachments.size()),
			last_subpass_where_used(spec.attachments.size())
		{
			FixedList<bool, MAX_ATTACHMENTS> first_use_tracked(spec.attachments.size(), false);

			auto usage_list_it = subpass_attachment_usage_lists.begin();
			for(uint8_t subpass_idx = 0; auto& subpass : spec.subpasses)
			{
				auto& usage_list = *usage_list_it++;

				usage_list = SubpassAttachmentUsageList(spec.attachments.size());
				for(uint8_t index : subpass.input_attachments)
				{
					usage_list[index].for_input = true;
					track_subpass_use(subpass_idx, index, first_use_tracked);
				}
				for(uint8_t index : subpass.output_attachments)
				{
					usage_list[index].for_output = true;
					track_subpass_use(subpass_idx, index, first_use_tracked);
				}
				for(uint8_t index : subpass.preserve_attachments)
					usage_list[index].preserve = true;

				++subpass_idx;
			}
		}

	private:
		void track_subpass_use(uint8_t subpass_idx, uint8_t attach_idx, FixedList<bool, MAX_ATTACHMENTS>& first_use_tracked)
		{
			if(!first_use_tracked[attach_idx])
			{
				first_subpass_where_used[attach_idx] = subpass_idx;
				first_use_tracked[attach_idx]		 = true;
			}
			last_subpass_where_used[attach_idx] = subpass_idx;
		}
	};

	export class D3D12RenderPass
	{
	public:
		D3D12RenderPass(RenderPassSpecification const& spec) :
			is_using_depth_stencil(spec.uses_depth_stencil_attachment()),
			formats(spec.attachments.size()),
			subpasses(spec.subpasses.size())
		{
			initialize_formats(spec);

			ResourceStateList		  current_layouts(spec);
			RenderPassAttachmentUsage attachment_usage(spec);

			for(uint8_t subpass_idx = 0; auto& subpass : subpasses)
			{
				subpass.flags = D3D12_RENDER_PASS_FLAG_NONE;

				auto& usage_list = attachment_usage.subpass_attachment_usage_lists[subpass_idx];
				for(uint8_t attach_idx = 0; auto [for_input, for_output, preserve] : usage_list)
				{
					bool const is_depth_stencil = is_using_depth_stencil && attach_idx == spec.attachments.size() - 1;

					// Initialize transitions
					D3D12_RESOURCE_STATES new_depth_layout, new_color_layout;
					if(for_input && for_output)
					{
						subpass.flags	 = D3D12_RENDER_PASS_FLAG_ALLOW_UAV_WRITES;
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
						new_color_layout = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
					}
					auto old_layout = current_layouts[attach_idx];
					auto new_layout = is_depth_stencil ? new_depth_layout : new_color_layout;
					if(old_layout != new_layout)
					{
						subpass.transitions.emplace_back(attach_idx, for_input && for_output, old_layout, new_layout);
						current_layouts[attach_idx] = new_layout;
					}

					// Initialize access list
					auto& new_access	 = subpass.accesses.emplace_back();
					new_access.for_input = for_input;

					uint8_t first_subpass_where_used = attachment_usage.first_subpass_where_used[attach_idx];
					if(subpass_idx < first_subpass_where_used || !for_output && !preserve)
						new_access.begin = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS;
					else if(subpass_idx == first_subpass_where_used)
						new_access.begin = convert_image_load_op(spec.attachments[attach_idx].load_op);
					else
						new_access.begin = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;

					uint8_t last_subpass_where_used = attachment_usage.last_subpass_where_used[attach_idx];
					if(subpass_idx > last_subpass_where_used || !for_output && !preserve)
						new_access.end = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS;
					else if(subpass_idx == last_subpass_where_used)
						new_access.end = convert_image_store_op(spec.attachments[attach_idx].store_op);
					else
						new_access.end = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;

					// Initialize stencil begin and end accesses
					if(is_depth_stencil)
					{
						if(subpass_idx == first_subpass_where_used)
							subpass.stencil_begin = convert_image_load_op(spec.stencil_load_op);
						else if(subpass_idx > first_subpass_where_used && (for_output || preserve))
							subpass.stencil_begin = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;

						if(subpass_idx == last_subpass_where_used)
							subpass.stencil_end = convert_image_store_op(spec.stencil_store_op);
						else if(subpass_idx < last_subpass_where_used && (for_output || preserve))
							subpass.stencil_end = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
					}
					++attach_idx;
				}
				++subpass_idx;
			}

			initialize_final_transitions(spec, current_layouts);
		}

		DXGI_FORMAT get_attachment_format(size_t index) const
		{
			return formats[index];
		}

		size_t count_color_attachments() const
		{
			return formats.size() - is_using_depth_stencil;
		}

		DXGI_FORMAT get_depth_stencil_format() const
		{
			return formats.back();
		}

		Subpass const& get_subpass(size_t index) const
		{
			return subpasses[index];
		}

		// None of these transitions should require a UAV barrier.
		TransitionList const& get_final_transitions() const
		{
			return final_transitions;
		}

		size_t count_subpasses() const
		{
			return subpasses.size();
		}

	private:
		bool									is_using_depth_stencil;
		FixedList<DXGI_FORMAT, MAX_ATTACHMENTS> formats;
		std::vector<Subpass>					subpasses;
		TransitionList							final_transitions;

		void initialize_formats(RenderPassSpecification const& spec)
		{
			auto format = formats.begin();
			for(auto attachment : spec.attachments)
				*format++ = convert_image_format(attachment.format);
		}

		void initialize_final_transitions(RenderPassSpecification const& spec, ResourceStateList const& current_layouts)
		{
			for(uint8_t i = 0; i != spec.attachments.size(); ++i)
			{
				auto prev  = current_layouts[i];
				auto final = convert_image_layout(spec.attachments[i].final_layout);
				if(prev != final)
					final_transitions.emplace_back(i, false, prev, final);
			}
		}
	};
}
