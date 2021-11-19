module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"
module vt.Graphics.D3D12.RenderPass;

import vt.Core.Array;
import vt.Core.Enum;
import vt.Core.FixedList;
import vt.Core.LookupTable;
import vt.Core.SmallList;
import vt.Graphics.D3D12.Image;
import vt.Graphics.RenderPassSpecification;

namespace vt::d3d12
{
	constexpr inline auto IMAGE_LOAD_OP_LOOKUP = [] {
		LookupTable<ImageLoadOp, D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE> _;
		using enum ImageLoadOp;

		_[Load]	  = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
		_[Clear]  = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
		_[Ignore] = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
		return _;
	}();

	constexpr inline auto IMAGE_STORE_OP_LOOKUP = [] {
		LookupTable<ImageStoreOp, D3D12_RENDER_PASS_ENDING_ACCESS_TYPE> _;
		using enum ImageStoreOp;

		_[Store]  = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
		_[Ignore] = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
		return _;
	}();

	bool Subpass::uses_depth_stencil() const
	{
		return stencil_begin != D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS;
	}

	ConstSpan<SubpassAccess> Subpass::get_color_attachment_accesses() const
	{
		return {accesses.begin(), accesses.size() - uses_depth_stencil()};
	}

	D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE Subpass::get_depth_begin_access() const
	{
		VT_ASSERT(uses_depth_stencil(), "This method is invalid unless a subpass uses the depth stencil attachment.");
		return accesses.back().begin;
	}

	D3D12_RENDER_PASS_ENDING_ACCESS_TYPE Subpass::get_depth_end_access() const
	{
		VT_ASSERT(uses_depth_stencil(), "This method is invalid unless a subpass uses the depth stencil attachment.");
		return accesses.back().end;
	}

	// Used only while creating a D3D12 render pass. Tracks what state a resource is expected to be in when a subpass begins.
	struct ResourceStateList : FixedList<D3D12_RESOURCE_STATES, MAX_ATTACHMENTS>
	{
		ResourceStateList(RenderPassSpecification const& spec) : FixedList(spec.attachments.size())
		{
			auto current_state = begin();
			for(auto attachment : spec.attachments)
			{
				D3D12_RESOURCE_STATES state;
				if(attachment.final_layout == ImageLayout::DepthStencilAttachment)
					state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
				else
					state = IMAGE_LAYOUT_LOOKUP[attachment.initial_layout];

				*current_state++ = state;
			}
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

	DXGI_FORMAT CommandListRenderPassData::get_attachment_format(size_t index) const
	{
		return formats[index];
	}

	size_t CommandListRenderPassData::count_color_attachments() const
	{
		return formats.size() - uses_depth_stencil;
	}

	DXGI_FORMAT CommandListRenderPassData::get_depth_stencil_format() const
	{
		VT_ASSERT(uses_depth_stencil, "This call is invalid on a render pass that never uses a depth stencil attachment.");
		return formats.back();
	}

	Subpass const& CommandListRenderPassData::get_subpass(size_t index) const
	{
		return subpasses[index];
	}

	// None of these transitions should require a UAV barrier.
	TransitionList const& CommandListRenderPassData::get_final_transitions() const
	{
		return final_transitions;
	}

	size_t CommandListRenderPassData::count_subpasses() const
	{
		return subpasses.size();
	}

	CommandListRenderPassData::CommandListRenderPassData(AttachmentFormatList const& formats,
														 Array<Subpass> const&		 subpasses,
														 TransitionList const&		 final_transitions,
														 bool						 uses_depth_stencil) :
		formats(formats), subpasses(subpasses), final_transitions(final_transitions), uses_depth_stencil(uses_depth_stencil)
	{}

	D3D12RenderPass::D3D12RenderPass(RenderPassSpecification const& spec) :
		formats(spec.attachments.size()),
		subpasses(spec.subpasses.size()),
		uses_depth_stencil(spec.uses_depth_stencil_attachment())
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
				bool const is_depth_stencil_index = uses_depth_stencil && attach_idx == spec.attachments.size() - 1;

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
				auto new_layout = is_depth_stencil_index ? new_depth_layout : new_color_layout;
				if(old_layout != new_layout)
				{
					bool needs_uav_barrier = for_input && for_output;
					subpass.transitions.emplace_back(attach_idx, needs_uav_barrier, old_layout, new_layout);
					current_layouts[attach_idx] = new_layout;
				}

				// Initialize access list
				auto& new_access	 = subpass.accesses.emplace_back();
				new_access.for_input = for_input;

				uint8_t first_subpass_where_used = attachment_usage.first_subpass_where_used[attach_idx];
				if(subpass_idx < first_subpass_where_used || !for_output && !preserve)
					new_access.begin = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS;
				else if(subpass_idx == first_subpass_where_used)
					new_access.begin = IMAGE_LOAD_OP_LOOKUP[spec.attachments[attach_idx].load_op];
				else
					new_access.begin = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;

				uint8_t last_subpass_where_used = attachment_usage.last_subpass_where_used[attach_idx];
				if(subpass_idx > last_subpass_where_used || !for_output && !preserve)
					new_access.end = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS;
				else if(subpass_idx == last_subpass_where_used)
					new_access.end = IMAGE_STORE_OP_LOOKUP[spec.attachments[attach_idx].store_op];
				else
					new_access.end = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;

				// Initialize stencil begin and end accesses
				if(is_depth_stencil_index)
				{
					if(subpass_idx == first_subpass_where_used)
						subpass.stencil_begin = IMAGE_LOAD_OP_LOOKUP[spec.stencil_load_op];
					else if(subpass_idx > first_subpass_where_used && (for_output || preserve))
						subpass.stencil_begin = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;

					if(subpass_idx == last_subpass_where_used)
						subpass.stencil_end = IMAGE_STORE_OP_LOOKUP[spec.stencil_store_op];
					else if(subpass_idx < last_subpass_where_used && (for_output || preserve))
						subpass.stencil_end = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
				}
				++attach_idx;
			}
			++subpass_idx;
		}

		initialize_final_transitions(spec, current_layouts);
	}

	CommandListRenderPassData D3D12RenderPass::get_data_for_command_list() const
	{
		return {
			formats,
			subpasses,
			final_transitions,
			uses_depth_stencil,
		};
	}

	DXGI_FORMAT D3D12RenderPass::get_attachment_format(size_t index) const
	{
		return formats[index];
	}

	DXGI_FORMAT D3D12RenderPass::get_depth_stencil_format() const
	{
		VT_ASSERT(uses_depth_stencil, "This call is invalid on a render pass that never uses a depth stencil attachment.");
		return formats.back();
	}

	Subpass const& D3D12RenderPass::get_subpass(size_t index) const
	{
		return subpasses[index];
	}

	void D3D12RenderPass::initialize_formats(RenderPassSpecification const& spec)
	{
		auto format = formats.begin();
		for(auto attachment : spec.attachments)
			*format++ = IMAGE_FORMAT_LOOKUP[attachment.format];
	}

	void D3D12RenderPass::initialize_final_transitions(RenderPassSpecification const&							spec,
													   FixedList<D3D12_RESOURCE_STATES, MAX_ATTACHMENTS> const& current_layouts)
	{
		for(uint8_t i = 0; i != spec.attachments.size(); ++i)
		{
			auto prev  = current_layouts[i];
			auto final = IMAGE_LAYOUT_LOOKUP[spec.attachments[i].final_layout];
			if(prev != final)
				final_transitions.emplace_back(i, false, prev, final);
		}
	}
}
