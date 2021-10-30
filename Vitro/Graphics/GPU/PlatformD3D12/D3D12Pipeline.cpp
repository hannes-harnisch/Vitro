module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.Pipeline;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.D3D12.Sampler;
import vt.Graphics.PipelineSpecification;

namespace vt::d3d12
{
	char const* convert_vertex_data_type_semantic(VertexDataType type)
	{
		using enum VertexDataType;
		switch(type)
		{ // clang-format off
			case Position:			  return "POSITION";
			case TransformedPosition: return "POSITIONT";
			case TextureCoordinates:  return "TEXCOORD";
			case Normal:			  return "NORMAL";
			case Binormal:			  return "BINORMAL";
			case Tangent:			  return "TANGENT";
			case Color:				  return "COLOR";
			case BlendIndices:		  return "BLENDINDICES";
			case BlendWeight:		  return "BLENDWEIGHT";
			case PointSize:			  return "PSIZE";
		}
		VT_UNREACHABLE();
	}

	DXGI_FORMAT convert_vertex_data_type_format(VertexDataType type)
	{
		using enum VertexDataType;
		switch(type)
		{
			case Position:
			case TransformedPosition:
			case TextureCoordinates:
			case Normal:
			case Binormal:
			case Tangent:
			case Color:		   return DXGI_FORMAT_R32G32B32A32_FLOAT;
			case PointSize:
			case BlendWeight:  return DXGI_FORMAT_R32_FLOAT;
			case BlendIndices: return DXGI_FORMAT_R32_UINT;
		}
		VT_UNREACHABLE();
	}

	D3D12_INPUT_CLASSIFICATION convert_input_rate(VertexBufferInputRate rate)
	{
		switch(rate)
		{
			case VertexBufferInputRate::PerVertex:	 return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			case VertexBufferInputRate::PerInstance: return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
		}
		VT_UNREACHABLE();
	}

	D3D12_FILL_MODE convert_fill_mode(PolygonFillMode mode)
	{
		switch(mode)
		{
			case PolygonFillMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
			case PolygonFillMode::Solid:	 return D3D12_FILL_MODE_SOLID;
		}
		VT_UNREACHABLE();
	}

	D3D12_CULL_MODE convert_cull_mode(CullMode mode)
	{
		using enum CullMode;
		switch(mode)
		{
			case None:  return D3D12_CULL_MODE_NONE;
			case Front: return D3D12_CULL_MODE_FRONT;
			case Back:  return D3D12_CULL_MODE_BACK;
		}
		VT_UNREACHABLE();
	}

	D3D12_STENCIL_OP convert_stencil_op(StencilOp op)
	{
		using enum StencilOp;
		switch(op)
		{
			case Keep:			 return D3D12_STENCIL_OP_KEEP;
			case Zero:			 return D3D12_STENCIL_OP_ZERO;
			case Replace:		 return D3D12_STENCIL_OP_REPLACE;
			case IncrementClamp: return D3D12_STENCIL_OP_INCR_SAT;
			case DecrementClamp: return D3D12_STENCIL_OP_DECR_SAT;
			case Invert:		 return D3D12_STENCIL_OP_INVERT;
			case IncrementWrap:	 return D3D12_STENCIL_OP_INCR;
			case DecrementWrap:	 return D3D12_STENCIL_OP_DECR;
		}
		VT_UNREACHABLE();
	}

	D3D12_LOGIC_OP convert_logic_op(LogicOp op)
	{
		using enum LogicOp;
		switch(op)
		{
			case Clear:		   return D3D12_LOGIC_OP_CLEAR;
			case Set:		   return D3D12_LOGIC_OP_SET;
			case Copy:		   return D3D12_LOGIC_OP_COPY;
			case CopyInverted: return D3D12_LOGIC_OP_COPY_INVERTED;
			case NoOp:		   return D3D12_LOGIC_OP_NOOP;
			case Invert:	   return D3D12_LOGIC_OP_INVERT;
			case And:		   return D3D12_LOGIC_OP_AND;
			case Nand:		   return D3D12_LOGIC_OP_NAND;
			case Or:		   return D3D12_LOGIC_OP_OR;
			case Nor:		   return D3D12_LOGIC_OP_NOR;
			case Xor:		   return D3D12_LOGIC_OP_XOR;
			case Equivalent:   return D3D12_LOGIC_OP_EQUIV;
			case AndReverse:   return D3D12_LOGIC_OP_AND_REVERSE;
			case AndInverted:  return D3D12_LOGIC_OP_AND_INVERTED;
			case OrReverse:	   return D3D12_LOGIC_OP_OR_REVERSE;
			case OrInverted:   return D3D12_LOGIC_OP_OR_INVERTED;
		}
		VT_UNREACHABLE();
	}

	D3D12_BLEND convert_blend_factor(BlendFactor factor)
	{
		using enum BlendFactor;
		switch(factor)
		{
			case Zero:			   return D3D12_BLEND_ZERO;
			case One:			   return D3D12_BLEND_ONE;
			case SrcColor:		   return D3D12_BLEND_SRC_COLOR;
			case SrcColorInv:	   return D3D12_BLEND_INV_SRC_COLOR;
			case SrcAlpha:		   return D3D12_BLEND_SRC_ALPHA;
			case SrcAlphaInv:	   return D3D12_BLEND_INV_SRC_ALPHA;
			case DstAlpha:		   return D3D12_BLEND_DEST_ALPHA;
			case DstAlphaInv:	   return D3D12_BLEND_INV_DEST_ALPHA;
			case DstColor:		   return D3D12_BLEND_DEST_COLOR;
			case DstColorInv:	   return D3D12_BLEND_INV_DEST_COLOR;
			case SrcAlphaSaturate: return D3D12_BLEND_SRC_ALPHA_SAT;
			case Src1Color:		   return D3D12_BLEND_SRC1_COLOR;
			case Src1ColorInv:	   return D3D12_BLEND_INV_SRC1_COLOR;
			case Src1Alpha:		   return D3D12_BLEND_SRC1_ALPHA;
			case Src1AlphaInv:	   return D3D12_BLEND_INV_SRC1_ALPHA;
		}
		VT_UNREACHABLE();
	}

	D3D12_BLEND_OP convert_blend_op(BlendOp op)
	{
		using enum BlendOp;
		switch(op)
		{
			case Add:			  return D3D12_BLEND_OP_ADD;
			case Subtract:		  return D3D12_BLEND_OP_SUBTRACT;
			case ReverseSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
			case Min:			  return D3D12_BLEND_OP_MIN;
			case Max:			  return D3D12_BLEND_OP_MAX;
		}
		VT_UNREACHABLE();
	}

	D3D12_PRIMITIVE_TOPOLOGY_TYPE categorize_primitive_topology(PrimitiveTopology topology)
	{
		using enum PrimitiveTopology;
		switch(topology)
		{
			case PointList:		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
			case LineList:
			case LineStrip:		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
			case TriangleList:
			case TriangleStrip:	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			case PatchList:		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		}
		VT_UNREACHABLE();
	}

	D3D_PRIMITIVE_TOPOLOGY convert_primitive_topology(PrimitiveTopology topology, unsigned control_point_count)
	{
		using enum PrimitiveTopology;
		switch(topology)
		{
			case PointList:		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
			case LineList:		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			case LineStrip:		return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
			case TriangleList:	return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			case TriangleStrip:	return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; // clang-format on
			case PatchList:
			{
				VT_ASSERT(control_point_count > 0 && control_point_count <= MAX_PATCH_LIST_CONTROL_POINTS,
						  "The number of control points is out of bounds.");
				unsigned patch_list_topology = D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST - 1 + control_point_count;
				return static_cast<D3D_PRIMITIVE_TOPOLOGY>(patch_list_topology);
			}
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

	class Pipeline
	{
	public:
		ID3D12PipelineState* get_handle() const
		{
			return pipeline.get();
		}

	protected:
		ComUnique<ID3D12PipelineState> pipeline;
	};

	// TODO: Make subobject types get default-initialized with the correct value instead of later. MSVC crashes when trying it.
	struct RenderPipelineStream
	{
		union
		{
			void* used_only_for_alignment_0;
			struct
			{
				D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_0;
				ID3D12RootSignature*				root_signature;
			};
		};
		union
		{
			void* used_only_for_alignment_1;
			struct
			{
				D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_1;
				D3D12_SHADER_BYTECODE				vertex_shader_bytecode;
			};
		};
		union
		{
			void* used_only_for_alignment_2;
			struct
			{
				D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_2;
				D3D12_SHADER_BYTECODE				fragment_shader_bytecode;
			};
		};
		union
		{
			void* used_only_for_alignment_3;
			struct
			{
				D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_3;
				D3D12_SHADER_BYTECODE				domain_shader_bytecode;
			};
		};
		union
		{
			void* used_only_for_alignment_4;
			struct
			{
				D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_4;
				D3D12_SHADER_BYTECODE				hull_shader_bytecode;
			};
		};
		union
		{
			void* used_only_for_alignment_5;
			struct
			{
				D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_5;
				D3D12_BLEND_DESC					blend_desc;
			};
		};
		union
		{
			void* used_only_for_alignment_6;
			struct
			{
				D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_6;
				UINT								sample_mask;
			};
		};
		union
		{
			void* used_only_for_alignment_7;
			struct
			{
				D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_7;
				D3D12_RASTERIZER_DESC				rasterizer_desc;
			};
		};
		union
		{
			void* used_only_for_alignment_8;
			struct
			{
				D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_8;
				D3D12_INPUT_LAYOUT_DESC				input_layout_desc;
			};
		};
		union
		{
			void* used_only_for_alignment_9;
			struct
			{
				D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_9;
				D3D12_INDEX_BUFFER_STRIP_CUT_VALUE	index_buffer_strip_cut_value;
			};
		};
		union
		{
			void* used_only_for_alignment_10;
			struct
			{
				D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_10;
				D3D12_PRIMITIVE_TOPOLOGY_TYPE		primitive_topology_type;
			};
		};
		union
		{
			void* used_only_for_alignment_11;
			struct
			{
				D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_11;
				D3D12_RT_FORMAT_ARRAY				render_target_formats;
			};
		};
		union
		{
			void* used_only_for_alignment_12;
			struct
			{
				D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_12;
				DXGI_FORMAT							depth_stencil_format;
			};
		};
		union
		{
			void* used_only_for_alignment_13;
			struct
			{
				D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_13;
				DXGI_SAMPLE_DESC					sample_desc;
			};
		};
		union
		{
			void* used_only_for_alignment_14;
			struct
			{
				D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type_14;
				D3D12_DEPTH_STENCIL_DESC1			depth_stencil_desc1;
			};
		};
	};

	export class D3D12RenderPipeline : public Pipeline
	{
	public:
		D3D12RenderPipeline(ID3D12Device4& device, RenderPipelineSpecification const& spec) :
			primitive_topology(convert_primitive_topology(spec.primitive_topology, spec.patch_list_control_point_count))
		{
			FixedList<D3D12_INPUT_ELEMENT_DESC, MAX_VERTEX_BUFFERS * MAX_VERTEX_ATTRIBUTES> input_element_descs;

			UINT index = 0;
			for(auto& buffer_binding : spec.vertex_buffer_bindings)
			{
				auto   input_rate = convert_input_rate(buffer_binding.input_rate);
				UINT   step_rate  = buffer_binding.input_rate == VertexBufferInputRate::PerInstance ? 1u : 0u;
				size_t offset	  = 0;
				for(auto attribute : buffer_binding.attributes)
				{
					input_element_descs.emplace_back(D3D12_INPUT_ELEMENT_DESC {
						.SemanticName		  = convert_vertex_data_type_semantic(attribute.type),
						.SemanticIndex		  = attribute.semantic_index,
						.Format				  = convert_vertex_data_type_format(attribute.type),
						.InputSlot			  = index,
						.AlignedByteOffset	  = static_cast<UINT>(offset),
						.InputSlotClass		  = input_rate,
						.InstanceDataStepRate = step_rate,
					});
					offset += get_vertex_data_type_size(attribute.type);
				}
				++index;
			}

			auto& pass = spec.render_pass.d3d12;

			RenderPipelineStream stream {
				.type_0					  = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE,
				.root_signature			  = spec.root_signature.d3d12.get_handle(),
				.type_1					  = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS,
				.vertex_shader_bytecode	  = spec.vertex_shader.d3d12.get_bytecode(),
				.type_2					  = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS,
				.fragment_shader_bytecode = spec.fragment_shader ? spec.fragment_shader->d3d12.get_bytecode()
																 : D3D12_SHADER_BYTECODE(),
				.type_3					  = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS,
				.domain_shader_bytecode	  = spec.domain_shader ? spec.domain_shader->d3d12.get_bytecode()
															   : D3D12_SHADER_BYTECODE(),
				.type_4					  = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS,
				.hull_shader_bytecode	  = spec.hull_shader ? spec.hull_shader->d3d12.get_bytecode() : D3D12_SHADER_BYTECODE(),
				.type_5					  = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND,
				.blend_desc {
					.AlphaToCoverageEnable	= spec.multisample.enable_alpha_to_coverage,
					.IndependentBlendEnable = false,
				},
				.type_6		 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK,
				.sample_mask = spec.multisample.sample_mask,
				.type_7		 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER,
				.rasterizer_desc {
					.FillMode			   = convert_fill_mode(spec.rasterizer.fill_mode),
					.CullMode			   = convert_cull_mode(spec.rasterizer.cull_mode),
					.FrontCounterClockwise = spec.rasterizer.winding_order == WindingOrder::CounterClockwise,
					.DepthBias			   = spec.rasterizer.depth_bias,
					.DepthBiasClamp		   = spec.rasterizer.depth_bias_clamp,
					.SlopeScaledDepthBias  = spec.rasterizer.depth_bias_slope,
					.DepthClipEnable	   = spec.rasterizer.enable_depth_clip,
					.ForcedSampleCount	   = spec.multisample.rasterizer_sample_count,
				},
				.type_8 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT,
				.input_layout_desc {
					.pInputElementDescs = input_element_descs.data(),
					.NumElements		= count(input_element_descs),
				},
				.type_9						  = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_IB_STRIP_CUT_VALUE,
				.index_buffer_strip_cut_value = spec.enable_primitive_restart ? D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF
																			  : D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
				.type_10					  = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY,
				.primitive_topology_type	  = categorize_primitive_topology(spec.primitive_topology),
				.type_11					  = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS,
				.render_target_formats {
					.NumRenderTargets = spec.blend.attachment_states.count(),
				},
				.type_12			  = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT,
				.depth_stencil_format = pass.uses_depth_stencil() ? pass.get_depth_stencil_format() : DXGI_FORMAT_UNKNOWN,
				.type_13			  = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC,
				.sample_desc {
					.Count = spec.multisample.sample_count,
				},
				.type_14 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL1,
				.depth_stencil_desc1 {
					.DepthEnable		   = spec.depth_stencil.enable_depth_test,
					.DepthWriteMask		   = spec.depth_stencil.enable_depth_write ? D3D12_DEPTH_WRITE_MASK_ALL
																				   : D3D12_DEPTH_WRITE_MASK_ZERO,
					.DepthFunc			   = convert_compare_op(spec.depth_stencil.depth_compare_op),
					.StencilEnable		   = spec.depth_stencil.enable_stencil_test,
					.StencilReadMask	   = spec.depth_stencil.stencil_read_mask,
					.StencilWriteMask	   = spec.depth_stencil.stencil_write_mask,
					.FrontFace			   = convert_stencil_op_state(spec.depth_stencil.front),
					.BackFace			   = convert_stencil_op_state(spec.depth_stencil.back),
					.DepthBoundsTestEnable = spec.depth_stencil.enable_depth_bounds_test,
				},
			};
			size_t i = 0;
			for(auto state : spec.blend.attachment_states)
				stream.blend_desc.RenderTarget[i++] = convert_color_attachment_blend_state(spec.blend, state);

			i = 0;
			for(auto access : pass.get_render_target_accesses())
				stream.render_target_formats.RTFormats[i++] = access.format;

			D3D12_PIPELINE_STATE_STREAM_DESC const desc {
				.SizeInBytes				   = sizeof stream,
				.pPipelineStateSubobjectStream = &stream,
			};
			auto result = device.CreatePipelineState(&desc, VT_COM_OUT(pipeline));
			VT_CHECK_RESULT(result, "Failed to create D3D12 render pipeline.");
		}

		D3D_PRIMITIVE_TOPOLOGY get_topology() const
		{
			return primitive_topology;
		}

	private:
		D3D_PRIMITIVE_TOPOLOGY primitive_topology;
	};

	export class D3D12ComputePipeline : public Pipeline
	{
	public:
		D3D12ComputePipeline(ID3D12Device4& device, ComputePipelineSpecification const& spec)
		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC const desc {
				.pRootSignature = spec.root_signature.d3d12.get_handle(),
				.CS				= spec.compute_shader.d3d12.get_bytecode(),
			};
			auto result = device.CreateComputePipelineState(&desc, VT_COM_OUT(pipeline));
			VT_CHECK_RESULT(result, "Failed to create D3D12 compute pipeline.");
		}
	};
}
