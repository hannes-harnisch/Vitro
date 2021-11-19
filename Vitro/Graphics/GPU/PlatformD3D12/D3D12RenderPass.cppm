module;
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.RenderPass;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Core.SmallList;
import vt.Graphics.RenderPassSpecification;

namespace vt::d3d12
{
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
	using SubpassAccessList = FixedList<SubpassAccess, MAX_ATTACHMENTS>;

	// Stores all information related to a specific subpass of a D3D12 render pass.
	struct Subpass
	{
		D3D12_RENDER_PASS_FLAGS					flags;
		TransitionList							transitions;
		SubpassAccessList						accesses;
		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE stencil_begin = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS;
		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE	stencil_end	  = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS;

		bool									uses_depth_stencil() const;
		ConstSpan<SubpassAccess>				get_color_attachment_accesses() const;
		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE get_depth_begin_access() const;
		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE	get_depth_end_access() const;
	};

	using AttachmentFormatList = FixedList<DXGI_FORMAT, MAX_ATTACHMENTS>;

	// Contains a copy of/view into render pass data for use in command lists, so we never have to store pointers to render
	// passes, which is unsafe in case a render pass gets moved. We don't need to guard against destruction, since that is
	// invalid no matter what and should be guarded against using a deferred deletion queue.
	export class CommandListRenderPassData
	{
		friend class D3D12RenderPass;

	public:
		CommandListRenderPassData() = default;

		DXGI_FORMAT			  get_attachment_format(size_t index) const;
		size_t				  count_color_attachments() const;
		DXGI_FORMAT			  get_depth_stencil_format() const;
		Subpass const&		  get_subpass(size_t index) const;
		TransitionList const& get_final_transitions() const; // None of these transitions should require a UAV barrier.
		size_t				  count_subpasses() const;

	private:
		AttachmentFormatList formats;
		ConstSpan<Subpass>	 subpasses;
		TransitionList		 final_transitions;
		bool				 uses_depth_stencil;

		CommandListRenderPassData(AttachmentFormatList const& formats,
								  Array<Subpass> const&		  subpasses,
								  TransitionList const&		  final_transitions,
								  bool						  uses_depth_stencil);
	};

	export class D3D12RenderPass
	{
	public:
		D3D12RenderPass(RenderPassSpecification const& spec);

		CommandListRenderPassData get_data_for_command_list() const;
		DXGI_FORMAT				  get_attachment_format(size_t index) const;
		DXGI_FORMAT				  get_depth_stencil_format() const;
		Subpass const&			  get_subpass(size_t index) const;

	private:
		AttachmentFormatList formats;
		Array<Subpass>		 subpasses;
		TransitionList		 final_transitions;
		bool				 uses_depth_stencil;

		void initialize_formats(RenderPassSpecification const& spec);
		void initialize_final_transitions(RenderPassSpecification const&						   spec,
										  FixedList<D3D12_RESOURCE_STATES, MAX_ATTACHMENTS> const& current_layouts);
	};
}
