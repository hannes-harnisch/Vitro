export module Vitro.Graphics.PipelineDescription;

import Vitro.Core.Array;
import Vitro.Core.Flags;
import Vitro.Graphics.Handle;
import Vitro.Graphics.RenderPass;
import Vitro.Graphics.RenderPassBase;

namespace vt
{
	export enum class Format : unsigned char {
		Unknown,
		R32_G32_B32_A32_Float,
		R32_G32_B32_A32_UInt,
		R32_G32_B32_A32_SInt,
		R32_G32_B32_Float,
		R32_G32_B32_UInt,
		R32_G32_B32_SInt,
		R16_G16_B16_A16_Float,
		R16_G16_B16_A16_UNorm,
		R16_G16_B16_A16_UInt,
		R16_G16_B16_A16_SNorm,
		R16_G16_B16_A16_SInt,
		R32_G32_Float,
		R32_G32_UInt,
		R32_G32_SInt,
		R32_G8_X24_Typeless,
		D32_Float_S8_X24_UInt,
		R10_G10_B10_A2_UNorm,
		R10_G10_B10_A2_UInt,
		R11_G11_B10_Float,
		R8_G8_B8_A8_UNorm,
		R8_G8_B8_A8_UNormSrgb,
		R8_G8_B8_A8_UInt,
		R8_G8_B8_A8_SNorm,
		R8_G8_B8_A8_SInt,
		B8_G8_R8_A8_UNorm,
		B8_G8_R8_A8_UNormSrgb,
		R16_G16_Float,
		R16_G16_UNorm,
		R16_G16_UInt,
		R16_G16_SNorm,
		R16_G16_SInt,
		R32_Typeless,
		D32_Float,
		R32_Float,
		R32_UInt,
		R32_SInt,
		R24_G8_Typeless,
		D24_UNorm_S8_UInt,
		R8_G8_UNorm,
		R8_G8_UInt,
		R8_G8_SNorm,
		R8_G8_SInt,
		R16_Typeless,
		R16_Float,
		D16_UNorm,
		R16_UNorm,
		R16_UInt,
		R16_SNorm,
		R16_SInt,
		R8_UNorm,
		R8_UInt,
		R8_SNorm,
		R8_SInt,
		Bc1_UNorm,
		Bc1_UNormSrgb,
		Bc2_UNorm,
		Bc2_UNormSrgb,
		Bc3_UNorm,
		Bc3_UNormSrgb,
		Bc4_UNorm,
		Bc4_SNorm,
		Bc5_UNorm,
		Bc5_SNorm,
		Bc6H_UFloat16,
		Bc6H_SFloat16,
		Bc7_UNorm,
		Bc7_UNormSrgb,
	};

	export enum class AttributeInputRate : unsigned char {
		PerVertex,
		PerInstance,
	};

	export struct VertexAttribute
	{
		unsigned char slot			 = 0;
		unsigned char byteOffset	 = 0;
		Format format				 = {};
		AttributeInputRate inputRate = {};
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
		PatchList,
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
		PolygonFillMode fillMode = {};
		CullMode cullMode		 = {};
		FrontFace frontFace		 = {};
		bool enableDepthClip	 = false;
		int depthBias			 = 0;
		float depthBiasClamp	 = 0;
		float depthBiasSlope	 = 0;
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
		unsigned char stencilReadMask		  = 0;
		unsigned char stencilWriteMask		  = 0;
		StencilOperationState front;
		StencilOperationState back;
	};

	struct MultisampleState
	{
		unsigned sampleMask					= 0;
		unsigned char sampleCount			= 0;
		unsigned char rasterizerSampleCount = 0;
		bool enableAlphaToCoverage			= {};
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
		BlendFactor srcColorFactor = BlendFactor::SrcAlpha;
		BlendFactor dstColorFactor = BlendFactor::SrcAlphaInv;
		BlendOperation colorOp	   = BlendOperation::Add;
		BlendFactor srcAlphaFactor = BlendFactor::One;
		BlendFactor dstAlphaFactor = BlendFactor::One;
		BlendOperation alphaOp	   = BlendOperation::Add;
		ColorComponent writeMask   = ColorComponent::All;
	};

	export struct BlendState
	{
		ColorAttachmentBlendState attachmentStates[RenderPassDescription::MaxColorAttachments];
		unsigned char attachmentCount = 0;
		bool enableLogicOp			  = false;
		LogicOperation logicOp		  = {};
	};

	export struct RenderPipelineDescription
	{
		static constexpr unsigned MaxVertexAttributes = 16;

		RootSignatureHandle rootSignature;
		RenderPass const& renderPass;
		Array<char> const& vertexShaderBytecode;
		Array<char> const& fragmentShaderBytecode;
		VertexAttribute vertexAttributes[MaxVertexAttributes];
		unsigned char vertexAttributeCount	= 0;
		PrimitiveTopology primitiveTopology = {};
		bool enablePrimitiveRestart			= false;
		RasterizerState rasterizer;
		DepthStencilState depthStencil;
		MultisampleState multisample;
		BlendState blend;
	};
}
