export module vt.Graphics.PipelineInfo;

import vt.Core.Array;
import vt.Core.Enum;
import vt.Core.FixedList;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderPassInfo;
import vt.Graphics.RootSignature;
import vt.Graphics.SamplerInfo;

namespace vt
{
	export enum class VertexDataType : unsigned char {
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

	export enum class AttributeInputRate : unsigned char {
		PerVertex,
		PerInstance,
	};

	export struct VertexAttribute
	{
		unsigned char	   slot		   = 0;
		unsigned char	   byte_offset = 0;
		VertexDataType	   data_type   = {};
		AttributeInputRate input_rate  = {};
	};

	export enum class PrimitiveTopology : unsigned char {
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip,
		LineListWithAdjacency,
		LineStripWithAdjacency,
		TriangleListWithAdjacency,
		TriangleStripWithAdjacency,
		PatchList_1ControlPoint,
		PatchList_2ControlPoints,
		PatchList_3ControlPoints,
		PatchList_4ControlPoints,
		PatchList_5ControlPoints,
		PatchList_6ControlPoints,
		PatchList_7ControlPoints,
		PatchList_8ControlPoints,
		PatchList_9ControlPoints,
		PatchList_10ControlPoints,
		PatchList_11ControlPoints,
		PatchList_12ControlPoints,
		PatchList_13ControlPoints,
		PatchList_14ControlPoints,
		PatchList_15ControlPoints,
		PatchList_16ControlPoints,
		PatchList_17ControlPoints,
		PatchList_18ControlPoints,
		PatchList_19ControlPoints,
		PatchList_20ControlPoints,
		PatchList_21ControlPoints,
		PatchList_22ControlPoints,
		PatchList_23ControlPoints,
		PatchList_24ControlPoints,
		PatchList_25ControlPoints,
		PatchList_26ControlPoints,
		PatchList_27ControlPoints,
		PatchList_28ControlPoints,
		PatchList_29ControlPoints,
		PatchList_30ControlPoints,
		PatchList_31ControlPoints,
		PatchList_32ControlPoints,
	};

	export enum class PolygonFillMode : unsigned char {
		Wireframe,
		Solid,
	};

	export enum class CullMode : unsigned char {
		None,
		Front,
		Back,
	};

	export enum class FrontFace : unsigned char {
		Clockwise,
		CounterClockwise,
	};

	struct RasterizerState
	{
		PolygonFillMode fill_mode		  = {};
		CullMode		cull_mode		  = {};
		FrontFace		front_face		  = {};
		bool			enable_depth_clip = false;
		int				depth_bias		  = 0;
		float			depth_bias_clamp  = 0;
		float			depth_bias_slope  = 0;
	};

	export enum class StencilOp : unsigned char {
		Keep,
		Zero,
		Replace,
		IncrementClamp,
		DecrementClamp,
		Invert,
		IncrementWrap,
		DecrementWrap,
	};

	export struct StencilOpState
	{
		StencilOp fail_op		= {};
		StencilOp pass_op		= {};
		StencilOp depth_fail_op = {};
		CompareOp compare_op	= {};
	};

	struct DepthStencilState
	{
		bool		   enable_depth_test   = false;
		bool		   enable_depth_write  = false;
		CompareOp	   depth_compare_op	   = {};
		bool		   enable_stencil_test = false;
		unsigned char  stencil_read_mask   = 0;
		unsigned char  stencil_write_mask  = 0;
		StencilOpState front;
		StencilOpState back;
	};

	struct MultisampleState
	{
		unsigned	  sample_mask			   = 0;
		unsigned char sample_count			   = 1;
		unsigned char rasterizer_sample_count  = 1;
		bool		  enable_alpha_to_coverage = {};
	};

	export enum class LogicOp : unsigned char {
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

	export enum class BlendFactor : unsigned char {
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

	export enum class BlendOp : unsigned char {
		Add,
		Subtract,
		ReverseSubtract,
		Min,
		Max,
	};

	export enum class ColorComponent : unsigned char {
		None  = 0,
		Red	  = bit(0),
		Green = bit(1),
		Blue  = bit(2),
		Alpha = bit(3),
		All	  = Red | Green | Blue | Alpha,
	};
	export template<> constexpr bool EnableBitOperatorsFor<ColorComponent> = true;

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
		FixedList<ColorAttachmentBlendState, MaxColorAttachments> attachment_states;

		bool	enable_logic_op = false;
		LogicOp logic_op		= {};
	};

	export constexpr unsigned MaxVertexAttributes = 16;

	export struct RenderPipelineInfo
	{
		using VertexAttributeList = FixedList<VertexAttribute, MaxVertexAttributes>;

		RootSignature const& root_signature;
		RenderPass const&	 render_pass;
		Array<char> const&	 vertex_shader_bytecode;
		Array<char> const&	 fragment_shader_bytecode;
		VertexAttributeList	 vertex_attributes;
		PrimitiveTopology	 primitive_topology		  = {};
		bool				 enable_primitive_restart = false;
		RasterizerState		 rasterizer;
		DepthStencilState	 depth_stencil;
		MultisampleState	 multisample;
		BlendState			 blend;
	};
}
