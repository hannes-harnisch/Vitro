module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"
export module vt.D3D12.Pipeline;

import vt.Core.Algorithm;
import vt.Core.FixedList;
import vt.D3D12.Utils;
import vt.Graphics.Device;
import vt.Graphics.PipelineInfo;

namespace vt::d3d12
{
	DXGI_FORMAT convert_vertex_data_type(VertexDataType type)
	{
		using enum VertexDataType;
		switch(type)
		{
			case Float: return DXGI_FORMAT_R32_FLOAT;
			case Float2: return DXGI_FORMAT_R32G32_FLOAT;
			case Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
			case Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
			case Int: return DXGI_FORMAT_R32_SINT;
			case Int2: return DXGI_FORMAT_R32G32_SINT;
			case Int3: return DXGI_FORMAT_R32G32B32_SINT;
			case Int4: return DXGI_FORMAT_R32G32B32A32_SINT;
			case UInt: return DXGI_FORMAT_R32_UINT;
			case UInt2: return DXGI_FORMAT_R32G32_UINT;
			case UInt3: return DXGI_FORMAT_R32G32B32_UINT;
			case UInt4: return DXGI_FORMAT_R32G32B32A32_UINT;
			case Bool: return DXGI_FORMAT_R8_UINT;
		}
		VT_UNREACHABLE();
	}

	D3D12_INPUT_CLASSIFICATION convert_attribute_input_rate(AttributeInputRate rate)
	{
		switch(rate)
		{
			case AttributeInputRate::PerVertex: return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			case AttributeInputRate::PerInstance: return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
		}
		VT_UNREACHABLE();
	}

	D3D12_INPUT_ELEMENT_DESC convert_vertex_attribute(VertexAttribute attrib)
	{
		return {
			.Format				  = convert_vertex_data_type(attrib.data_type),
			.InputSlot			  = attrib.slot,
			.AlignedByteOffset	  = attrib.byte_offset,
			.InputSlotClass		  = convert_attribute_input_rate(attrib.input_rate),
			.InstanceDataStepRate = attrib.input_rate == AttributeInputRate::PerInstance ? 1u : 0u,
		};
	}

	D3D12_PRIMITIVE_TOPOLOGY_TYPE categorize_primitive_topology(PrimitiveTopology topology)
	{
		using enum PrimitiveTopology;
		switch(topology)
		{
			case PointList: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
			case LineList:
			case LineStrip:
			case LineListWithAdjacency:
			case LineStripWithAdjacency: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
			case TriangleList:
			case TriangleStrip:
			case TriangleListWithAdjacency:
			case TriangleStripWithAdjacency: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			case PatchList_1ControlPoint:
			case PatchList_2ControlPoints:
			case PatchList_3ControlPoints:
			case PatchList_4ControlPoints:
			case PatchList_5ControlPoints:
			case PatchList_6ControlPoints:
			case PatchList_7ControlPoints:
			case PatchList_8ControlPoints:
			case PatchList_9ControlPoints:
			case PatchList_10ControlPoints:
			case PatchList_11ControlPoints:
			case PatchList_12ControlPoints:
			case PatchList_13ControlPoints:
			case PatchList_14ControlPoints:
			case PatchList_15ControlPoints:
			case PatchList_16ControlPoints:
			case PatchList_17ControlPoints:
			case PatchList_18ControlPoints:
			case PatchList_19ControlPoints:
			case PatchList_20ControlPoints:
			case PatchList_21ControlPoints:
			case PatchList_22ControlPoints:
			case PatchList_23ControlPoints:
			case PatchList_24ControlPoints:
			case PatchList_25ControlPoints:
			case PatchList_26ControlPoints:
			case PatchList_27ControlPoints:
			case PatchList_28ControlPoints:
			case PatchList_29ControlPoints:
			case PatchList_30ControlPoints:
			case PatchList_31ControlPoints:
			case PatchList_32ControlPoints: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		}
		VT_UNREACHABLE();
	}

	D3D12_FILL_MODE convert_fill_mode(PolygonFillMode mode)
	{
		switch(mode)
		{
			case PolygonFillMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
			case PolygonFillMode::Solid: return D3D12_FILL_MODE_SOLID;
		}
		VT_UNREACHABLE();
	}

	D3D12_CULL_MODE convert_cull_mode(CullMode mode)
	{
		switch(mode)
		{
			case CullMode::None: return D3D12_CULL_MODE_NONE;
			case CullMode::Front: return D3D12_CULL_MODE_FRONT;
			case CullMode::Back: return D3D12_CULL_MODE_BACK;
		}
		VT_UNREACHABLE();
	}

	D3D12_COMPARISON_FUNC convert_compare_op(CompareOp op)
	{
		using enum CompareOp;
		switch(op)
		{
			case Never: return D3D12_COMPARISON_FUNC_NEVER;
			case Less: return D3D12_COMPARISON_FUNC_LESS;
			case Equal: return D3D12_COMPARISON_FUNC_EQUAL;
			case LessOrEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
			case Greater: return D3D12_COMPARISON_FUNC_GREATER;
			case NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
			case GreaterOrEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
			case Always: return D3D12_COMPARISON_FUNC_ALWAYS;
		}
		VT_UNREACHABLE();
	}

	D3D12_STENCIL_OP convert_stencil_op(StencilOp op)
	{
		using enum StencilOp;
		switch(op)
		{
			case Keep: return D3D12_STENCIL_OP_KEEP;
			case Zero: return D3D12_STENCIL_OP_ZERO;
			case Replace: return D3D12_STENCIL_OP_REPLACE;
			case IncrementClamp: return D3D12_STENCIL_OP_INCR_SAT;
			case DecrementClamp: return D3D12_STENCIL_OP_DECR_SAT;
			case Invert: return D3D12_STENCIL_OP_INVERT;
			case IncrementWrap: return D3D12_STENCIL_OP_INCR;
			case DecrementWrap: return D3D12_STENCIL_OP_DECR;
		}
		VT_UNREACHABLE();
	}

	D3D12_DEPTH_STENCILOP_DESC convert_stencil_op_state(StencilOpState op_state)
	{
		return {
			.StencilFailOp		= convert_stencil_op(op_state.fail_op),
			.StencilDepthFailOp = convert_stencil_op(op_state.depth_fail_op),
			.StencilPassOp		= convert_stencil_op(op_state.pass_op),
			.StencilFunc		= convert_compare_op(op_state.compare_op),
		};
	}

	D3D12_LOGIC_OP convert_logic_op(LogicOp op)
	{
		using enum LogicOp;
		switch(op)
		{
			case Clear: return D3D12_LOGIC_OP_CLEAR;
			case Set: return D3D12_LOGIC_OP_SET;
			case Copy: return D3D12_LOGIC_OP_COPY;
			case CopyInverted: return D3D12_LOGIC_OP_COPY_INVERTED;
			case NoOp: return D3D12_LOGIC_OP_NOOP;
			case Invert: return D3D12_LOGIC_OP_INVERT;
			case And: return D3D12_LOGIC_OP_AND;
			case Nand: return D3D12_LOGIC_OP_NAND;
			case Or: return D3D12_LOGIC_OP_OR;
			case Nor: return D3D12_LOGIC_OP_NOR;
			case Xor: return D3D12_LOGIC_OP_XOR;
			case Equivalent: return D3D12_LOGIC_OP_EQUIV;
			case AndReverse: return D3D12_LOGIC_OP_AND_REVERSE;
			case AndInverted: return D3D12_LOGIC_OP_AND_INVERTED;
			case OrReverse: return D3D12_LOGIC_OP_OR_REVERSE;
			case OrInverted: return D3D12_LOGIC_OP_OR_INVERTED;
		}
		VT_UNREACHABLE();
	}

	D3D12_BLEND convert_blend_factor(BlendFactor factor)
	{
		using enum BlendFactor;
		switch(factor)
		{
			case Zero: return D3D12_BLEND_ZERO;
			case One: return D3D12_BLEND_ONE;
			case SrcColor: return D3D12_BLEND_SRC_COLOR;
			case SrcColorInv: return D3D12_BLEND_INV_SRC_COLOR;
			case SrcAlpha: return D3D12_BLEND_SRC_ALPHA;
			case SrcAlphaInv: return D3D12_BLEND_INV_SRC_ALPHA;
			case DstAlpha: return D3D12_BLEND_DEST_ALPHA;
			case DstAlphaInv: return D3D12_BLEND_INV_DEST_ALPHA;
			case DstColor: return D3D12_BLEND_DEST_COLOR;
			case DstColorInv: return D3D12_BLEND_INV_DEST_COLOR;
			case SrcAlphaSaturate: return D3D12_BLEND_SRC_ALPHA_SAT;
			case Src1Color: return D3D12_BLEND_SRC1_COLOR;
			case Src1ColorInv: return D3D12_BLEND_INV_SRC1_COLOR;
			case Src1Alpha: return D3D12_BLEND_SRC1_ALPHA;
			case Src1AlphaInv: return D3D12_BLEND_INV_SRC1_ALPHA;
		}
		VT_UNREACHABLE();
	}

	D3D12_BLEND_OP convert_blend_op(BlendOp op)
	{
		using enum BlendOp;
		switch(op)
		{
			case Add: return D3D12_BLEND_OP_ADD;
			case Subtract: return D3D12_BLEND_OP_SUBTRACT;
			case ReverseSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
			case Min: return D3D12_BLEND_OP_MIN;
			case Max: return D3D12_BLEND_OP_MAX;
		}
		VT_UNREACHABLE();
	}

	D3D12_RENDER_TARGET_BLEND_DESC convert_color_attachment_blend_state(BlendState const&		  blend,
																		ColorAttachmentBlendState state)
	{
		return {
			.BlendEnable		   = state.enable_blend,
			.LogicOpEnable		   = blend.enable_logic_op,
			.SrcBlend			   = convert_blend_factor(state.src_color_factor),
			.DestBlend			   = convert_blend_factor(state.dst_color_factor),
			.BlendOp			   = convert_blend_op(state.color_op),
			.SrcBlendAlpha		   = convert_blend_factor(state.src_alpha_factor),
			.DestBlendAlpha		   = convert_blend_factor(state.dst_alpha_factor),
			.BlendOpAlpha		   = convert_blend_op(state.alpha_op),
			.LogicOp			   = convert_logic_op(blend.logic_op),
			.RenderTargetWriteMask = static_cast<UINT8>(state.write_mask),
		};
	}

	export class D3D12Pipeline
	{
	public:
		D3D12Pipeline(Device const& device, RenderPipelineInfo const& info)
		{
			FixedList<D3D12_INPUT_ELEMENT_DESC, MaxVertexAttributes> input_element_descs;
			for(auto attrib : info.vertex_attributes)
				input_element_descs.emplace_back(convert_vertex_attribute(attrib));

			auto& pass = info.render_pass.d3d12;

			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc {
				.pRootSignature = info.root_signature.d3d12.ptr(),
				.VS {
					.pShaderBytecode = info.vertex_shader_bytecode.data(),
					.BytecodeLength	 = info.vertex_shader_bytecode.size(),
				},
				.PS {
					.pShaderBytecode = info.fragment_shader_bytecode.data(),
					.BytecodeLength	 = info.fragment_shader_bytecode.size(),
				},
				.BlendState {
					.AlphaToCoverageEnable	= info.multisample.enable_alpha_to_coverage,
					.IndependentBlendEnable = false,
				},
				.SampleMask = info.multisample.sample_mask,
				.RasterizerState {
					.FillMode			   = convert_fill_mode(info.rasterizer.fill_mode),
					.CullMode			   = convert_cull_mode(info.rasterizer.cull_mode),
					.FrontCounterClockwise = info.rasterizer.front_face == FrontFace::CounterClockwise,
					.DepthBias			   = info.rasterizer.depth_bias,
					.DepthBiasClamp		   = info.rasterizer.depth_bias_clamp,
					.SlopeScaledDepthBias  = info.rasterizer.depth_bias_slope,
					.DepthClipEnable	   = info.rasterizer.enable_depth_clip,
					.ForcedSampleCount	   = info.multisample.rasterizer_sample_count,
				},
				.DepthStencilState {
					.DepthEnable	  = info.depth_stencil.enable_depth_test,
					.DepthWriteMask	  = info.depth_stencil.enable_depth_write ? D3D12_DEPTH_WRITE_MASK_ALL
																			  : D3D12_DEPTH_WRITE_MASK_ZERO,
					.DepthFunc		  = convert_compare_op(info.depth_stencil.depth_compare_op),
					.StencilEnable	  = info.depth_stencil.enable_stencil_test,
					.StencilReadMask  = info.depth_stencil.stencil_read_mask,
					.StencilWriteMask = info.depth_stencil.stencil_write_mask,
					.FrontFace		  = convert_stencil_op_state(info.depth_stencil.front),
					.BackFace		  = convert_stencil_op_state(info.depth_stencil.back),
				},
				.InputLayout {
					.pInputElementDescs = input_element_descs.data(),
					.NumElements		= count(input_element_descs),
				},
				.IBStripCutValue	   = info.enable_primitive_restart ? D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF
																	   : D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
				.PrimitiveTopologyType = categorize_primitive_topology(info.primitive_topology),
				.NumRenderTargets	   = info.blend.attachment_states.count(),
				.DSVFormat			   = pass.uses_depth_stencil() ? pass.get_depth_stencil_format() : DXGI_FORMAT_UNKNOWN,
				.SampleDesc			   = {.Count = info.multisample.sample_count},
			};
			unsigned i = 0;
			for(auto state : info.blend.attachment_states)
				desc.BlendState.RenderTarget[i++] = convert_color_attachment_blend_state(info.blend, state);

			i = 0;
			for(auto attachment : pass.get_render_target_attachments())
				desc.RTVFormats[i++] = attachment.format;

			ID3D12PipelineState* raw_pipeline;

			auto result = device.d3d12.ptr()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&raw_pipeline));
			pipeline.reset(raw_pipeline);
			VT_ASSERT_RESULT(result, "Failed to create render pipeline.");
		}

		ID3D12PipelineState* ptr() const
		{
			return pipeline.get();
		}

	private:
		ComUnique<ID3D12PipelineState> pipeline;
	};
}
