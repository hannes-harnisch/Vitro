﻿module;
#include <cstdint>
export module vt.Graphics.PipelineSpecification;

import vt.Core.Array;
import vt.Core.Enum;
import vt.Core.FixedList;
import vt.Core.Specification;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderPassSpecification;
import vt.Graphics.RootSignature;

namespace vt
{
	export enum class VertexDataType : uint8_t {
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		Int2,
		Int3,
		Int4,
		UInt,
		UInt2,
		UInt3,
		UInt4,
		Bool,
	};

	export enum class AttributeInputRate : uint8_t {
		PerVertex,
		PerInstance,
	};

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

	export enum class CullMode : uint8_t {
		None,
		Front,
		Back,
	};

	export enum class FrontFace : uint8_t {
		Clockwise,
		CounterClockwise,
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
	export template<> constexpr inline bool ALLOW_BIT_OPERATORS_FOR<ColorComponent> = true;

	export struct VertexAttribute
	{
		Explicit<uint8_t>		 slot;
		Explicit<uint8_t>		 byte_offset;
		Explicit<VertexDataType> data_type;
		AttributeInputRate		 input_rate = AttributeInputRate::PerVertex;
	};

	struct RasterizerState
	{
		PolygonFillMode		fill_mode = PolygonFillMode::Solid;
		Explicit<CullMode>	cull_mode;
		Explicit<FrontFace> front_face;
		bool				enable_depth_clip = false;
		int					depth_bias		  = 0;
		float				depth_bias_clamp  = 0.0f;
		float				depth_bias_slope  = 0.0f;
	};

	export struct StencilOpState
	{
		StencilOp fail_op		= StencilOp::Keep;
		StencilOp pass_op		= StencilOp::Keep;
		StencilOp depth_fail_op = StencilOp::Keep;
		CompareOp compare_op	= CompareOp::Never;
	};

	struct DepthStencilState
	{
		Explicit<bool> enable_depth_test;
		bool		   enable_depth_write		= false;
		CompareOp	   depth_compare_op			= CompareOp::Never;
		bool		   enable_depth_bounds_test = false;
		Explicit<bool> enable_stencil_test;
		uint8_t		   stencil_read_mask  = 0;
		uint8_t		   stencil_write_mask = 0;
		StencilOpState front;
		StencilOpState back;
	};

	struct MultisampleState
	{
		unsigned		  sample_mask			   = ~0u;
		Positive<uint8_t> sample_count			   = 1;
		Positive<uint8_t> rasterizer_sample_count  = 1;
		bool			  enable_alpha_to_coverage = false;
	};

	export struct ColorAttachmentBlendState
	{
		bool		   enable_blend		= false;
		BlendFactor	   src_color_factor = BlendFactor::SrcAlpha;
		BlendFactor	   dst_color_factor = BlendFactor::SrcAlphaInv;
		BlendOp		   color_op			= BlendOp::Add;
		BlendFactor	   src_alpha_factor = BlendFactor::One;
		BlendFactor	   dst_alpha_factor = BlendFactor::One;
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

	export constexpr inline unsigned MAX_VERTEX_ATTRIBUTES		   = 16;
	export constexpr inline unsigned MAX_PATCH_LIST_CONTROL_POINTS = 32; // Imposed by D3D12 primitive topology.

	export struct RenderPipelineSpecification
	{
		using VertexAttributeList = FixedList<VertexAttribute, MAX_VERTEX_ATTRIBUTES>;

		RootSignature const&		root_signature;
		RenderPass const&			render_pass;
		ArrayView<char>				vertex_shader_bytecode;
		ConstSpan<char>				hull_shader_bytecode;
		ConstSpan<char>				domain_shader_bytecode;
		ConstSpan<char>				fragment_shader_bytecode;
		VertexAttributeList			vertex_attributes;
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
		ArrayView<char>		 compute_shader_bytecode;
	};
}