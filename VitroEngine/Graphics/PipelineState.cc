export module Vitro.Graphics.PipelineState;

import Vitro.Core.Array;
import Vitro.Core.Flags;
import Vitro.Graphics.Handle;

namespace vt
{
	export enum class PrimitiveTopology : unsigned char {
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip,
	};

	export enum class AttributeInputRate : unsigned char {
		PerVertex,
		PerInstance,
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

	struct StencilOperationState
	{
		StencilOperation failOp		  = {};
		StencilOperation passOp		  = {};
		StencilOperation depthFailOp  = {};
		ComparisonOperation compareOp = {};
	};

	struct DepthStencilState
	{
		bool enableDepthTest				  = false;
		bool enableDepthWrite				  = false;
		ComparisonOperation depthComparisonOp = {};
		bool enableStencilTest				  = false;
		unsigned char stencilReadMask		  = {};
		unsigned char stencilWriteMask		  = {};
		StencilOperationState front;
		StencilOperationState back;
	};

	struct MultisampleState
	{
		bool enableAlphaToCoverage = {};
		unsigned char sampleCount  = {};
		unsigned sampleMask		   = {};
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

	struct ColorAttachmentBlendState
	{
		bool enableBlend		   = false;
		BlendFactor srcColorFactor = {};
		BlendFactor dstColorFactor = {};
		BlendOperation colorOp	   = {};
		BlendFactor srcAlphaFactor = {};
		BlendFactor dstAlphaFactor = {};
		BlendOperation alphaOp	   = {};
		ColorComponent writeMask   = {};
	};

	struct BlendState
	{
		static constexpr unsigned MaxColorAttachments = 8;

		bool enableLogicOp	   = false;
		LogicOperation logicOp = {};
		ColorAttachmentBlendState attachmentStates[MaxColorAttachments];
	};

	export struct PipelineState
	{
		PrimitiveTopology primitiveTopology = {};

		RootSignatureHandle rootSignature;

		Array<char> vertexShaderBytecode;
		DepthStencilState depthStencil;
		Array<char> fragmentShaderBytecode;
		MultisampleState multisample;
		BlendState blend;
	};
}
