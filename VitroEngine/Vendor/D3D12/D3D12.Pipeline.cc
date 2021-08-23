module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"
export module Vitro.D3D12.Pipeline;

import Vitro.Core.Algorithm;
import Vitro.Core.FixedList;
import Vitro.D3D12.Utils;
import Vitro.Graphics.Device;
import Vitro.Graphics.PipelineInfo;

namespace vt::d3d12
{
	constexpr DXGI_FORMAT convertVertexDataType(VertexDataType type)
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
		vtUnreachable();
	}

	constexpr D3D12_INPUT_CLASSIFICATION convertAttributeInputRate(AttributeInputRate rate)
	{
		switch(rate)
		{
			case AttributeInputRate::PerVertex: return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			case AttributeInputRate::PerInstance: return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
		}
		vtUnreachable();
	}

	constexpr D3D12_INPUT_ELEMENT_DESC convertVertexAttribute(VertexAttribute attrib)
	{
		return {
			.Format				  = convertVertexDataType(attrib.dataType),
			.InputSlot			  = attrib.slot,
			.AlignedByteOffset	  = attrib.byteOffset,
			.InputSlotClass		  = convertAttributeInputRate(attrib.inputRate),
			.InstanceDataStepRate = attrib.inputRate == AttributeInputRate::PerInstance ? 1u : 0u,
		};
	}

	constexpr D3D12_PRIMITIVE_TOPOLOGY_TYPE categorizePrimitiveTopology(PrimitiveTopology topology)
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
		vtUnreachable();
	}

	constexpr D3D12_FILL_MODE convertFillMode(PolygonFillMode mode)
	{
		switch(mode)
		{
			case PolygonFillMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
			case PolygonFillMode::Solid: return D3D12_FILL_MODE_SOLID;
		}
		vtUnreachable();
	}

	constexpr D3D12_CULL_MODE convertCullMode(CullMode mode)
	{
		switch(mode)
		{
			case CullMode::None: return D3D12_CULL_MODE_NONE;
			case CullMode::Front: return D3D12_CULL_MODE_FRONT;
			case CullMode::Back: return D3D12_CULL_MODE_BACK;
		}
		vtUnreachable();
	}

	constexpr D3D12_COMPARISON_FUNC convertCompareOp(CompareOp op)
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
		vtUnreachable();
	}

	constexpr D3D12_STENCIL_OP convertStencilOp(StencilOp op)
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
		vtUnreachable();
	}

	constexpr D3D12_DEPTH_STENCILOP_DESC convertStencilOpState(StencilOpState opState)
	{
		return {
			.StencilFailOp		= convertStencilOp(opState.failOp),
			.StencilDepthFailOp = convertStencilOp(opState.depthFailOp),
			.StencilPassOp		= convertStencilOp(opState.passOp),
			.StencilFunc		= convertCompareOp(opState.compareOp),
		};
	}

	constexpr D3D12_LOGIC_OP convertLogicOp(LogicOp op)
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
		vtUnreachable();
	}

	constexpr D3D12_BLEND convertBlendFactor(BlendFactor factor)
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
		vtUnreachable();
	}

	constexpr D3D12_BLEND_OP convertBlendOp(BlendOp op)
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
		vtUnreachable();
	}

	D3D12_RENDER_TARGET_BLEND_DESC convertColorAttachmentBlendState(BlendState const& blend, ColorAttachmentBlendState state)
	{
		return {
			.BlendEnable		   = state.enableBlend,
			.LogicOpEnable		   = blend.enableLogicOp,
			.SrcBlend			   = convertBlendFactor(state.srcColorFactor),
			.DestBlend			   = convertBlendFactor(state.dstColorFactor),
			.BlendOp			   = convertBlendOp(state.colorOp),
			.SrcBlendAlpha		   = convertBlendFactor(state.srcAlphaFactor),
			.DestBlendAlpha		   = convertBlendFactor(state.dstAlphaFactor),
			.BlendOpAlpha		   = convertBlendOp(state.alphaOp),
			.LogicOp			   = convertLogicOp(blend.logicOp),
			.RenderTargetWriteMask = static_cast<UINT8>(state.writeMask),
		};
	}

	export class Pipeline
	{
	public:
		Pipeline(vt::Device const& device, RenderPipelineInfo const& info) :
			pipeline(makeRenderPipeline(device.d3d12.get(), info))
		{}

		ID3D12PipelineState* get() const
		{
			return pipeline.get();
		}

	private:
		ComUnique<ID3D12PipelineState> pipeline;

		static decltype(pipeline) makeRenderPipeline(ID3D12Device4* device, RenderPipelineInfo const& info)
		{
			FixedList<D3D12_INPUT_ELEMENT_DESC, MaxVertexAttributes> inputElementDescs;
			for(auto attrib : info.vertexAttributes)
				inputElementDescs.emplace_back(convertVertexAttribute(attrib));

			auto& pass = info.renderPass.d3d12;

			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc {
				.pRootSignature = info.rootSignature.d3d12.get(),
				.VS {
					.pShaderBytecode = info.vertexShaderBytecode.data(),
					.BytecodeLength	 = info.vertexShaderBytecode.size(),
				},
				.PS {
					.pShaderBytecode = info.fragmentShaderBytecode.data(),
					.BytecodeLength	 = info.fragmentShaderBytecode.size(),
				},
				.BlendState {
					.AlphaToCoverageEnable	= info.multisample.enableAlphaToCoverage,
					.IndependentBlendEnable = false,
				},
				.SampleMask = info.multisample.sampleMask,
				.RasterizerState {
					.FillMode			   = convertFillMode(info.rasterizer.fillMode),
					.CullMode			   = convertCullMode(info.rasterizer.cullMode),
					.FrontCounterClockwise = info.rasterizer.frontFace == FrontFace::CounterClockwise,
					.DepthBias			   = info.rasterizer.depthBias,
					.DepthBiasClamp		   = info.rasterizer.depthBiasClamp,
					.SlopeScaledDepthBias  = info.rasterizer.depthBiasSlope,
					.DepthClipEnable	   = info.rasterizer.enableDepthClip,
					.ForcedSampleCount	   = info.multisample.rasterizerSampleCount,
				},
				.DepthStencilState {
					.DepthEnable	  = info.depthStencil.enableDepthTest,
					.DepthWriteMask	  = info.depthStencil.enableDepthWrite ? D3D12_DEPTH_WRITE_MASK_ALL
																		   : D3D12_DEPTH_WRITE_MASK_ZERO,
					.DepthFunc		  = convertCompareOp(info.depthStencil.depthCompareOp),
					.StencilEnable	  = info.depthStencil.enableStencilTest,
					.StencilReadMask  = info.depthStencil.stencilReadMask,
					.StencilWriteMask = info.depthStencil.stencilWriteMask,
					.FrontFace		  = convertStencilOpState(info.depthStencil.front),
					.BackFace		  = convertStencilOpState(info.depthStencil.back),
				},
				.InputLayout {
					.pInputElementDescs = inputElementDescs.data(),
					.NumElements		= count(inputElementDescs),
				},
				.IBStripCutValue	   = info.enablePrimitiveRestart ? D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF
																	 : D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
				.PrimitiveTopologyType = categorizePrimitiveTopology(info.primitiveTopology),
				.NumRenderTargets	   = info.blend.attachmentStates.count(),
				.DSVFormat			   = pass.usesDepthStencil ? pass.attachments.back().format : DXGI_FORMAT_UNKNOWN,
				.SampleDesc			   = {.Count = info.multisample.sampleCount},
			};
			unsigned i = 0;
			for(auto state : info.blend.attachmentStates)
				desc.BlendState.RenderTarget[i++] = convertColorAttachmentBlendState(info.blend, state);

			i = 0;
			for(auto attachment : std::span(pass.attachments.begin(), pass.attachments.end() - pass.usesDepthStencil))
				desc.RTVFormats[i++] = attachment.format;

			ID3D12PipelineState* rawPipeline;

			auto result = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&rawPipeline));

			decltype(pipeline) freshPipeline(rawPipeline);
			vtAssertResult(result, "Failed to create render pipeline.");

			return freshPipeline;
		}
	};
}
