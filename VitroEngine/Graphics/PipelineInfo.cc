export module Vitro.Graphics.PipelineInfo;

import Vitro.Core.Array;
import Vitro.Core.Enum;
import Vitro.Core.FixedList;
import Vitro.Graphics.RenderPass;
import Vitro.Graphics.RenderPassInfo;
import Vitro.Graphics.RootSignature;

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
		unsigned char	   slot		  = 0;
		unsigned char	   byteOffset = 0;
		VertexDataType	   dataType	  = {};
		AttributeInputRate inputRate  = {};
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
		PolygonFillMode fillMode		= {};
		CullMode		cullMode		= {};
		FrontFace		frontFace		= {};
		bool			enableDepthClip = false;
		int				depthBias		= 0;
		float			depthBiasClamp	= 0;
		float			depthBiasSlope	= 0;
	};

	export enum class ComparisonOperation : unsigned char {
		Never,
		Less,
		Equal,
		LessOrEqual,
		Greater,
		NotEqual,
		GreaterOrEqual,
		Always,
	};

	export enum class StencilOperation : unsigned char {
		Keep,
		Zero,
		Replace,
		IncrementClamp,
		DecrementClamp,
		Invert,
		IncrementWrap,
		DecrementWrap,
	};

	export struct StencilOperationState
	{
		StencilOperation	failOp		= {};
		StencilOperation	passOp		= {};
		StencilOperation	depthFailOp = {};
		ComparisonOperation compareOp	= {};
	};

	struct DepthStencilState
	{
		bool				  enableDepthTest	= false;
		bool				  enableDepthWrite	= false;
		ComparisonOperation	  depthComparisonOp = {};
		bool				  enableStencilTest = false;
		unsigned char		  stencilReadMask	= 0;
		unsigned char		  stencilWriteMask	= 0;
		StencilOperationState front;
		StencilOperationState back;
	};

	struct MultisampleState
	{
		unsigned	  sampleMask			= 0;
		unsigned char sampleCount			= 1;
		unsigned char rasterizerSampleCount = 1;
		bool		  enableAlphaToCoverage = {};
	};

	export enum class LogicOperation : unsigned char {
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

	export enum class BlendOperation : unsigned char {
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
	export template<> constexpr bool EnableFlagsFor<ColorComponent> = true;

	export struct ColorAttachmentBlendState
	{
		bool		   enableBlend	  = false;
		BlendFactor	   srcColorFactor = BlendFactor::SrcAlpha;
		BlendFactor	   dstColorFactor = BlendFactor::SrcAlphaInv;
		BlendOperation colorOp		  = BlendOperation::Add;
		BlendFactor	   srcAlphaFactor = BlendFactor::One;
		BlendFactor	   dstAlphaFactor = BlendFactor::One;
		BlendOperation alphaOp		  = BlendOperation::Add;
		ColorComponent writeMask	  = ColorComponent::All;
	};

	export struct BlendState
	{
		FixedList<ColorAttachmentBlendState, MaxColorAttachments> attachmentStates;
		bool													  enableLogicOp = false;
		LogicOperation											  logicOp		= {};
	};

	export constexpr unsigned MaxVertexAttributes = 16;

	export struct RenderPipelineInfo
	{
		RootSignature const&							rootSignature;
		RenderPass const&								renderPass;
		Array<char> const&								vertexShaderBytecode;
		Array<char> const&								fragmentShaderBytecode;
		FixedList<VertexAttribute, MaxVertexAttributes> vertexAttributes;
		PrimitiveTopology								primitiveTopology	   = {};
		bool											enablePrimitiveRestart = false;
		RasterizerState									rasterizer;
		DepthStencilState								depthStencil;
		MultisampleState								multisample;
		BlendState										blend;
	};
}
