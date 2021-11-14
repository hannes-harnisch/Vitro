module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"
export module vt.Graphics.D3D12.Pipeline;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Core.LookupTable;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.D3D12.Sampler;
import vt.Graphics.PipelineSpecification;
import vt.Graphics.Shader;

namespace vt::d3d12
{
	constexpr inline auto VERTEX_DATA_TYPE_SEMANTIC_LOOKUP = [] {
		LookupTable<VertexDataType, char const*> _;
		using enum VertexDataType;

		_[Position]			   = "POSITION";
		_[TransformedPosition] = "POSITIONT";
		_[TextureCoordinates]  = "TEXCOORD";
		_[Normal]			   = "NORMAL";
		_[Binormal]			   = "BINORMAL";
		_[Tangent]			   = "TANGENT";
		_[Color]			   = "COLOR";
		_[BlendIndices]		   = "BLENDINDICES";
		_[BlendWeight]		   = "BLENDWEIGHT";
		_[PointSize]		   = "PSIZE";
		return _;
	}();

	constexpr inline auto VERTEX_DATA_TYPE_LOOKUP = [] {
		LookupTable<VertexDataType, DXGI_FORMAT> _;
		using enum VertexDataType;

		_[Position]			   = DXGI_FORMAT_R32G32B32A32_FLOAT;
		_[TransformedPosition] = _[Position];
		_[TextureCoordinates]  = _[Position];
		_[Normal]			   = _[Position];
		_[Binormal]			   = _[Position];
		_[Tangent]			   = _[Position];
		_[Color]			   = _[Position];
		_[PointSize]		   = DXGI_FORMAT_R32_FLOAT;
		_[BlendWeight]		   = DXGI_FORMAT_R32_FLOAT;
		_[BlendIndices]		   = DXGI_FORMAT_R32_UINT;
		return _;
	}();

	constexpr inline auto FILL_MODE_LOOKUP = [] {
		LookupTable<PolygonFillMode, D3D12_FILL_MODE> _;
		using enum PolygonFillMode;

		_[Wireframe] = D3D12_FILL_MODE_WIREFRAME;
		_[Solid]	 = D3D12_FILL_MODE_SOLID;
		return _;
	}();

	constexpr inline auto CULL_MODE_LOOKUP = [] {
		LookupTable<CullMode, D3D12_CULL_MODE> _;
		using enum CullMode;

		_[None]	 = D3D12_CULL_MODE_NONE;
		_[Front] = D3D12_CULL_MODE_FRONT;
		_[Back]	 = D3D12_CULL_MODE_BACK;
		return _;
	}();

	constexpr inline auto STENCIL_OP_LOOKUP = [] {
		LookupTable<StencilOp, D3D12_STENCIL_OP> _;
		using enum StencilOp;

		_[Keep]			  = D3D12_STENCIL_OP_KEEP;
		_[Zero]			  = D3D12_STENCIL_OP_ZERO;
		_[Replace]		  = D3D12_STENCIL_OP_REPLACE;
		_[IncrementClamp] = D3D12_STENCIL_OP_INCR_SAT;
		_[DecrementClamp] = D3D12_STENCIL_OP_DECR_SAT;
		_[Invert]		  = D3D12_STENCIL_OP_INVERT;
		_[IncrementWrap]  = D3D12_STENCIL_OP_INCR;
		_[DecrementWrap]  = D3D12_STENCIL_OP_DECR;
		return _;
	}();

	constexpr inline auto LOGIC_OP_LOOKUP = [] {
		LookupTable<LogicOp, D3D12_LOGIC_OP> _;
		using enum LogicOp;

		_[Clear]		= D3D12_LOGIC_OP_CLEAR;
		_[Set]			= D3D12_LOGIC_OP_SET;
		_[Copy]			= D3D12_LOGIC_OP_COPY;
		_[CopyInverted] = D3D12_LOGIC_OP_COPY_INVERTED;
		_[NoOp]			= D3D12_LOGIC_OP_NOOP;
		_[Invert]		= D3D12_LOGIC_OP_INVERT;
		_[And]			= D3D12_LOGIC_OP_AND;
		_[Nand]			= D3D12_LOGIC_OP_NAND;
		_[Or]			= D3D12_LOGIC_OP_OR;
		_[Nor]			= D3D12_LOGIC_OP_NOR;
		_[Xor]			= D3D12_LOGIC_OP_XOR;
		_[Equivalent]	= D3D12_LOGIC_OP_EQUIV;
		_[AndReverse]	= D3D12_LOGIC_OP_AND_REVERSE;
		_[AndInverted]	= D3D12_LOGIC_OP_AND_INVERTED;
		_[OrReverse]	= D3D12_LOGIC_OP_OR_REVERSE;
		_[OrInverted]	= D3D12_LOGIC_OP_OR_INVERTED;
		return _;
	}();

	constexpr inline auto BLEND_FACTOR_LOOKUP = [] {
		LookupTable<BlendFactor, D3D12_BLEND> _;
		using enum BlendFactor;

		_[Zero]				= D3D12_BLEND_ZERO;
		_[One]				= D3D12_BLEND_ONE;
		_[SrcColor]			= D3D12_BLEND_SRC_COLOR;
		_[SrcColorInv]		= D3D12_BLEND_INV_SRC_COLOR;
		_[SrcAlpha]			= D3D12_BLEND_SRC_ALPHA;
		_[SrcAlphaInv]		= D3D12_BLEND_INV_SRC_ALPHA;
		_[DstAlpha]			= D3D12_BLEND_DEST_ALPHA;
		_[DstAlphaInv]		= D3D12_BLEND_INV_DEST_ALPHA;
		_[DstColor]			= D3D12_BLEND_DEST_COLOR;
		_[DstColorInv]		= D3D12_BLEND_INV_DEST_COLOR;
		_[SrcAlphaSaturate] = D3D12_BLEND_SRC_ALPHA_SAT;
		_[Src1Color]		= D3D12_BLEND_SRC1_COLOR;
		_[Src1ColorInv]		= D3D12_BLEND_INV_SRC1_COLOR;
		_[Src1Alpha]		= D3D12_BLEND_SRC1_ALPHA;
		_[Src1AlphaInv]		= D3D12_BLEND_INV_SRC1_ALPHA;
		return _;
	}();

	constexpr inline auto BLEND_OP_LOOKUP = [] {
		LookupTable<BlendOp, D3D12_BLEND_OP> _;
		using enum BlendOp;

		_[Add]			   = D3D12_BLEND_OP_ADD;
		_[Subtract]		   = D3D12_BLEND_OP_SUBTRACT;
		_[ReverseSubtract] = D3D12_BLEND_OP_REV_SUBTRACT;
		_[Min]			   = D3D12_BLEND_OP_MIN;
		_[Max]			   = D3D12_BLEND_OP_MAX;
		return _;
	}();

	constexpr inline auto PRIMITIVE_TOPOLOGY_CATEGORIES = [] {
		LookupTable<PrimitiveTopology, D3D12_PRIMITIVE_TOPOLOGY_TYPE> _;
		using enum PrimitiveTopology;

		_[PointList]	 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		_[LineList]		 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		_[LineStrip]	 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		_[TriangleList]	 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		_[TriangleStrip] = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		_[PatchList]	 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		return _;
	}();

	constexpr inline auto PRIMITIVE_TOPOLOGY_LOOKUP = [] {
		LookupTable<PrimitiveTopology, D3D_PRIMITIVE_TOPOLOGY> _;
		using enum PrimitiveTopology;

		_[PointList]	 = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		_[LineList]		 = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		_[LineStrip]	 = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
		_[TriangleList]	 = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		_[TriangleStrip] = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		return _;
	}();

	D3D_PRIMITIVE_TOPOLOGY convert_primitive_topology(PrimitiveTopology topology, unsigned control_point_count)
	{
		if(topology != PrimitiveTopology::PatchList)
			return PRIMITIVE_TOPOLOGY_LOOKUP[topology];
		else
		{
			VT_ASSERT(control_point_count > 0 && control_point_count <= MAX_PATCH_LIST_CONTROL_POINTS,
					  "The number of control points is out of bounds.");
			unsigned patch_list_topology = D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST - 1 + control_point_count;
			return static_cast<D3D_PRIMITIVE_TOPOLOGY>(patch_list_topology);
		}
	}

	D3D12_DEPTH_STENCILOP_DESC convert_stencil_op_state(StencilOpState op_state)
	{
		return {
			.StencilFailOp		= STENCIL_OP_LOOKUP[op_state.fail_op],
			.StencilDepthFailOp = STENCIL_OP_LOOKUP[op_state.depth_fail_op],
			.StencilPassOp		= STENCIL_OP_LOOKUP[op_state.pass_op],
			.StencilFunc		= COMPARE_OP_LOOKUP[op_state.compare_op],
		};
	}

	D3D12_RENDER_TARGET_BLEND_DESC convert_color_attachment_blend_state(BlendState const&		  blend,
																		ColorAttachmentBlendState state)
	{
		return {
			.BlendEnable		   = state.enable_blend,
			.LogicOpEnable		   = blend.enable_logic_op,
			.SrcBlend			   = BLEND_FACTOR_LOOKUP[state.src_color_factor],
			.DestBlend			   = BLEND_FACTOR_LOOKUP[state.dst_color_factor],
			.BlendOp			   = BLEND_OP_LOOKUP[state.color_op],
			.SrcBlendAlpha		   = BLEND_FACTOR_LOOKUP[state.src_alpha_factor],
			.DestBlendAlpha		   = BLEND_FACTOR_LOOKUP[state.dst_alpha_factor],
			.BlendOpAlpha		   = BLEND_OP_LOOKUP[state.alpha_op],
			.LogicOp			   = LOGIC_OP_LOOKUP[blend.logic_op],
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

	// TODO: Wait for compiler fix, then make subobject types get default-initialized with the correct value instead of later.
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

	D3D12_SHADER_BYTECODE get_optional_bytecode(Shader const* shader)
	{
		if(shader)
			return shader->d3d12.get_bytecode();

		return {};
	}

	export class D3D12RenderPipeline : public Pipeline
	{
	public:
		D3D12RenderPipeline(RenderPipelineSpecification const& spec, ID3D12Device4& device) :
			primitive_topology(convert_primitive_topology(spec.primitive_topology, spec.patch_list_control_point_count))
		{
			FixedList<D3D12_INPUT_ELEMENT_DESC, MAX_VERTEX_BUFFERS * MAX_VERTEX_ATTRIBUTES> input_element_descs;

			UINT index = 0;
			for(auto& buffer_binding : spec.vertex_buffer_bindings)
			{
				D3D12_INPUT_CLASSIFICATION input_rate;
				UINT					   step_rate;
				if(buffer_binding.input_rate == VertexBufferInputRate::PerVertex)
				{
					input_rate = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
					step_rate  = 0;
				}
				else
				{
					input_rate = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
					step_rate  = 1;
				}
				size_t offset = 0;
				for(auto attribute : buffer_binding.attributes)
				{
					input_element_descs.emplace_back(D3D12_INPUT_ELEMENT_DESC {
						.SemanticName		  = VERTEX_DATA_TYPE_SEMANTIC_LOOKUP[attribute.type],
						.SemanticIndex		  = attribute.semantic_index,
						.Format				  = VERTEX_DATA_TYPE_LOOKUP[attribute.type],
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
				.fragment_shader_bytecode = get_optional_bytecode(spec.fragment_shader),
				.type_3					  = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS,
				.domain_shader_bytecode	  = get_optional_bytecode(spec.domain_shader),
				.type_4					  = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS,
				.hull_shader_bytecode	  = get_optional_bytecode(spec.hull_shader),
				.type_5					  = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND,
				.blend_desc {
					.AlphaToCoverageEnable	= spec.multisample.enable_alpha_to_coverage,
					.IndependentBlendEnable = false,
				},
				.type_6		 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK,
				.sample_mask = spec.multisample.sample_mask,
				.type_7		 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER,
				.rasterizer_desc {
					.FillMode			   = FILL_MODE_LOOKUP[spec.rasterizer.fill_mode],
					.CullMode			   = CULL_MODE_LOOKUP[spec.rasterizer.cull_mode],
					.FrontCounterClockwise = spec.rasterizer.winding_order == WindingOrder::CounterClockwise,
					.DepthBias			   = spec.rasterizer.depth_bias,
					.DepthBiasClamp		   = spec.rasterizer.depth_bias_clamp,
					.SlopeScaledDepthBias  = spec.rasterizer.depth_bias_slope,
					.DepthClipEnable	   = false,
					.ForcedSampleCount	   = 0,
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
				.primitive_topology_type	  = PRIMITIVE_TOPOLOGY_CATEGORIES[spec.primitive_topology],
				.type_11					  = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS,
				.render_target_formats {
					.NumRenderTargets = spec.blend.attachment_states.count(),
				},
				.type_12			  = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT,
				.depth_stencil_format = pass.get_subpass(spec.subpass_index).uses_depth_stencil()
											? pass.get_depth_stencil_format()
											: DXGI_FORMAT_UNKNOWN,
				.type_13			  = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC,
				.sample_desc {
					.Count = spec.multisample.sample_count,
				},
				.type_14 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL1,
				.depth_stencil_desc1 {
					.DepthEnable		   = spec.depth_stencil.enable_depth_test,
					.DepthWriteMask		   = spec.depth_stencil.enable_depth_write ? D3D12_DEPTH_WRITE_MASK_ALL
																				   : D3D12_DEPTH_WRITE_MASK_ZERO,
					.DepthFunc			   = COMPARE_OP_LOOKUP[spec.depth_stencil.depth_compare_op],
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
			for(; i != spec.blend.attachment_states.size(); ++i)
				stream.render_target_formats.RTFormats[i] = pass.get_attachment_format(i);

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
		D3D12ComputePipeline(ComputePipelineSpecification const& spec, ID3D12Device4& device)
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
