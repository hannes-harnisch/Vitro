module;
#include <cstdint>
export module vt.Graphics.PipelineSpecification;

import vt.Core.Array;
import vt.Core.Enum;
import vt.Core.FixedList;
import vt.Core.Specification;
import vt.Core.Vector;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderPassSpecification;
import vt.Graphics.RootSignature;
import vt.Graphics.Shader;

namespace vt
{
	// The kind of data represented by a vertex attribute.
	export enum class VertexDataType : uint8_t {
		Position,
		TransformedPosition,
		TextureCoordinates,
		Normal,
		Binormal,
		Tangent,
		Color,
		PointSize,
		BlendWeight,
		BlendIndices,
	};

	// Specifies whether a vertex buffer binding holds per-vertex or per-instance data.
	export enum class VertexBufferInputRate : uint8_t {
		PerVertex,
		PerInstance,
	};

	// The kind of primitive to be drawn.
	export enum class PrimitiveTopology : uint8_t {
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip,
		PatchList,
	};

	export enum class PolygonFillMode : uint8_t {
		Wireframe,
		Solid,
	};

	// What direction primitives need to face to be culled.
	export enum class CullMode : uint8_t {
		None, // Disables primitive culling.
		Front,
		Back,
	};

	export enum class WindingOrder : uint8_t {
		CounterClockwise,
		Clockwise,
	};

	export enum class StencilOp : uint8_t {
		Keep,
		Zero,
		Replace,
		IncrementClamp,
		DecrementClamp,
		Invert,
		IncrementWrap,
		DecrementWrap,
	};

	export enum class LogicOp : uint8_t {
		Clear,
		Set,
		Copy,
		CopyInverted,
		NoOp,
		Invert,
		And,
		Nand,
		Or,
		Nor,
		Xor,
		Equivalent,
		AndReverse,
		AndInverted,
		OrReverse,
		OrInverted,
	};

	export enum class BlendFactor : uint8_t {
		Zero,
		One,
		SrcColor,
		SrcColorInv,
		SrcAlpha,
		SrcAlphaInv,
		DstAlpha,
		DstAlphaInv,
		DstColor,
		DstColorInv,
		SrcAlphaSaturate,
		Src1Color,
		Src1ColorInv,
		Src1Alpha,
		Src1AlphaInv,
	};

	export enum class BlendOp : uint8_t {
		Add,
		Subtract,
		ReverseSubtract,
		Min,
		Max,
	};

	export enum class ColorComponent : uint8_t {
		None  = 0,
		Red	  = bit(0),
		Green = bit(1),
		Blue  = bit(2),
		Alpha = bit(3),
		All	  = Red | Green | Blue | Alpha,
	};
	export template<> constexpr inline bool ENABLE_BIT_OPERATORS_FOR<ColorComponent> = true;

	export constexpr inline unsigned MAX_VERTEX_ATTRIBUTES		   = 14; // Arbitrarily chosen.
	export constexpr inline unsigned MAX_VERTEX_BUFFERS			   = 16; // Imposed by D3D12 input assembly stage.
	export constexpr inline unsigned MAX_PATCH_LIST_CONTROL_POINTS = 32; // Imposed by D3D12 primitive topology.

	export struct VertexAttribute
	{
		Explicit<VertexDataType> type;
		uint8_t					 semantic_index = 0;
	};

	export struct VertexBufferBinding
	{
		using AttributeList = FixedList<VertexAttribute, MAX_VERTEX_ATTRIBUTES>;

		VertexBufferInputRate	input_rate = VertexBufferInputRate::PerVertex;
		Explicit<AttributeList> attributes;
	};

	struct RasterizerState
	{
		PolygonFillMode		   fill_mode = PolygonFillMode::Solid;
		Explicit<CullMode>	   cull_mode;
		Explicit<WindingOrder> winding_order;
		int					   depth_bias		= 0;
		float				   depth_bias_clamp = 0.0f;
		float				   depth_bias_slope = 0.0f;
	};

	export struct StencilOpState
	{
		StencilOp fail_op		= StencilOp::Keep;
		StencilOp pass_op		= StencilOp::Keep;
		StencilOp depth_fail_op = StencilOp::Keep;
		CompareOp compare_op	= CompareOp::Always;
	};

	struct DepthStencilState
	{
		bool		   enable_depth_test		= true;
		bool		   enable_depth_write		= true;
		CompareOp	   depth_compare_op			= CompareOp::Less;
		bool		   enable_depth_bounds_test = false;
		bool		   enable_stencil_test		= false;
		uint8_t		   stencil_read_mask		= 0xFF;
		uint8_t		   stencil_write_mask		= 0xFF;
		StencilOpState front;
		StencilOpState back;
	};

	struct MultisampleState
	{
		unsigned		  sample_mask			   = ~0u;
		Positive<uint8_t> sample_count			   = 1;
		bool			  enable_alpha_to_coverage = false;
	};

	export struct ColorAttachmentBlendState
	{
		bool		   enable_blend		= true;
		BlendFactor	   src_color_factor = BlendFactor::SrcAlpha;
		BlendFactor	   dst_color_factor = BlendFactor::SrcAlphaInv;
		BlendOp		   color_op			= BlendOp::Add;
		BlendFactor	   src_alpha_factor = BlendFactor::One;
		BlendFactor	   dst_alpha_factor = BlendFactor::Zero;
		BlendOp		   alpha_op			= BlendOp::Add;
		ColorComponent write_mask		= ColorComponent::All;
	};

	export struct BlendState
	{
		using BlendStateList = FixedList<ColorAttachmentBlendState, MAX_COLOR_ATTACHMENTS>;

		Explicit<BlendStateList> attachment_states;
		bool					 enable_logic_op = false;
		LogicOp					 logic_op		 = LogicOp::Clear;
	};

	export struct RenderPipelineSpecification
	{
		using VertexBufferBindingList = FixedList<VertexBufferBinding, MAX_VERTEX_BUFFERS>;

		RootSignature const&		root_signature;
		RenderPass const&			render_pass;
		Shader const&				vertex_shader;
		Shader const*				hull_shader		= nullptr;
		Shader const*				domain_shader	= nullptr;
		Shader const*				fragment_shader = nullptr;
		VertexBufferBindingList		vertex_buffer_bindings;
		Explicit<PrimitiveTopology> primitive_topology;
		uint8_t						patch_list_control_point_count = 0;
		bool						enable_primitive_restart	   = false;
		Explicit<uint8_t>			subpass_index;
		RasterizerState				rasterizer;
		DepthStencilState			depth_stencil;
		MultisampleState			multisample;
		BlendState					blend;
	};

	export struct ComputePipelineSpecification
	{
		RootSignature const& root_signature;
		Shader const&		 compute_shader;
	};

	export size_t get_vertex_data_type_size(VertexDataType type)
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
			case Color: return sizeof(Float4);
			case PointSize:
			case BlendWeight: return sizeof(float);
			case BlendIndices: return sizeof(unsigned);
		}
		return 0;
	}
}
