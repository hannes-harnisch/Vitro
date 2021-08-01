module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"

#include <ranges>
#include <span>
export module Vitro.D3D12.Device;

import Vitro.D3D12.Queue;
import Vitro.D3D12.Utils;
import Vitro.Graphics.Adapter;
import Vitro.Graphics.DeferredUnique;
import Vitro.Graphics.DeviceBase;
import Vitro.Graphics.Driver;
import Vitro.Graphics.Handle;
import Vitro.Graphics.PipelineDescription;

namespace vt::d3d12
{
	constexpr DXGI_FORMAT convertFormat(Format format)
	{
		using enum Format;
		switch(format)
		{
			case Unknown: return DXGI_FORMAT_UNKNOWN;
			case R32_G32_B32_A32_Float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
			case R32_G32_B32_A32_UInt: return DXGI_FORMAT_R32G32B32A32_UINT;
			case R32_G32_B32_A32_SInt: return DXGI_FORMAT_R32G32B32A32_SINT;
			case R32_G32_B32_Float: return DXGI_FORMAT_R32G32B32_FLOAT;
			case R32_G32_B32_UInt: return DXGI_FORMAT_R32G32B32_UINT;
			case R32_G32_B32_SInt: return DXGI_FORMAT_R32G32B32_SINT;
			case R16_G16_B16_A16_Float: return DXGI_FORMAT_R16G16B16A16_FLOAT;
			case R16_G16_B16_A16_UNorm: return DXGI_FORMAT_R16G16B16A16_UNORM;
			case R16_G16_B16_A16_UInt: return DXGI_FORMAT_R16G16B16A16_UINT;
			case R16_G16_B16_A16_SNorm: return DXGI_FORMAT_R16G16B16A16_SNORM;
			case R16_G16_B16_A16_SInt: return DXGI_FORMAT_R16G16B16A16_SINT;
			case R32_G32_Float: return DXGI_FORMAT_R32G32_FLOAT;
			case R32_G32_UInt: return DXGI_FORMAT_R32G32_UINT;
			case R32_G32_SInt: return DXGI_FORMAT_R32G32_SINT;
			case R32_G8_X24_Typeless: return DXGI_FORMAT_R32G8X24_TYPELESS;
			case D32_Float_S8_X24_UInt: return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
			case R10_G10_B10_A2_UNorm: return DXGI_FORMAT_R10G10B10A2_UNORM;
			case R10_G10_B10_A2_UInt: return DXGI_FORMAT_R10G10B10A2_UINT;
			case R11_G11_B10_Float: return DXGI_FORMAT_R11G11B10_FLOAT;
			case R8_G8_B8_A8_UNorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
			case R8_G8_B8_A8_UNormSrgb: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			case R8_G8_B8_A8_UInt: return DXGI_FORMAT_R8G8B8A8_UINT;
			case R8_G8_B8_A8_SNorm: return DXGI_FORMAT_R8G8B8A8_SNORM;
			case R8_G8_B8_A8_SInt: return DXGI_FORMAT_R8G8B8A8_SINT;
			case B8_G8_R8_A8_UNorm: return DXGI_FORMAT_B8G8R8A8_UNORM;
			case B8_G8_R8_A8_UNormSrgb: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
			case R16_G16_Float: return DXGI_FORMAT_R16G16_FLOAT;
			case R16_G16_UNorm: return DXGI_FORMAT_R16G16_UNORM;
			case R16_G16_UInt: return DXGI_FORMAT_R16G16_UINT;
			case R16_G16_SNorm: return DXGI_FORMAT_R16G16_SNORM;
			case R16_G16_SInt: return DXGI_FORMAT_R16G16_SINT;
			case R32_Typeless: return DXGI_FORMAT_R32_TYPELESS;
			case D32_Float: return DXGI_FORMAT_D32_FLOAT;
			case R32_Float: return DXGI_FORMAT_R32_FLOAT;
			case R32_UInt: return DXGI_FORMAT_R32_UINT;
			case R32_SInt: return DXGI_FORMAT_R32_SINT;
			case R24_G8_Typeless: return DXGI_FORMAT_R24G8_TYPELESS;
			case D24_UNorm_S8_UInt: return DXGI_FORMAT_D24_UNORM_S8_UINT;
			case R8_G8_UNorm: return DXGI_FORMAT_R8G8_UNORM;
			case R8_G8_UInt: return DXGI_FORMAT_R8G8_UINT;
			case R8_G8_SNorm: return DXGI_FORMAT_R8G8_SNORM;
			case R8_G8_SInt: return DXGI_FORMAT_R8G8_SINT;
			case R16_Typeless: return DXGI_FORMAT_R16_TYPELESS;
			case R16_Float: return DXGI_FORMAT_R16_FLOAT;
			case D16_UNorm: return DXGI_FORMAT_D16_UNORM;
			case R16_UNorm: return DXGI_FORMAT_R16_UNORM;
			case R16_UInt: return DXGI_FORMAT_R16_UINT;
			case R16_SNorm: return DXGI_FORMAT_R16_SNORM;
			case R16_SInt: return DXGI_FORMAT_R16_SINT;
			case R8_UNorm: return DXGI_FORMAT_R8_UNORM;
			case R8_UInt: return DXGI_FORMAT_R8_UINT;
			case R8_SNorm: return DXGI_FORMAT_R8_SNORM;
			case R8_SInt: return DXGI_FORMAT_R8_SINT;
			case Bc1_UNorm: return DXGI_FORMAT_BC1_UNORM;
			case Bc1_UNormSrgb: return DXGI_FORMAT_BC1_UNORM_SRGB;
			case Bc2_UNorm: return DXGI_FORMAT_BC2_UNORM;
			case Bc2_UNormSrgb: return DXGI_FORMAT_BC2_UNORM_SRGB;
			case Bc3_UNorm: return DXGI_FORMAT_BC3_UNORM;
			case Bc3_UNormSrgb: return DXGI_FORMAT_BC3_UNORM_SRGB;
			case Bc4_UNorm: return DXGI_FORMAT_BC4_UNORM;
			case Bc4_SNorm: return DXGI_FORMAT_BC4_SNORM;
			case Bc5_UNorm: return DXGI_FORMAT_BC5_UNORM;
			case Bc5_SNorm: return DXGI_FORMAT_BC5_SNORM;
			case Bc6H_UFloat16: return DXGI_FORMAT_BC6H_UF16;
			case Bc6H_SFloat16: return DXGI_FORMAT_BC6H_SF16;
			case Bc7_UNorm: return DXGI_FORMAT_BC7_UNORM;
			case Bc7_UNormSrgb: return DXGI_FORMAT_BC7_UNORM_SRGB;
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
			.Format				  = convertFormat(attrib.format),
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
			case PatchList: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
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

	constexpr D3D12_COMPARISON_FUNC convertComparisonOp(ComparisonOperation op)
	{
		using enum ComparisonOperation;
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

	constexpr D3D12_STENCIL_OP convertStencilOp(StencilOperation op)
	{
		using enum StencilOperation;
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

	constexpr D3D12_DEPTH_STENCILOP_DESC convertStencilOpState(StencilOperationState opState)
	{
		return {
			.StencilFailOp		= convertStencilOp(opState.failOp),
			.StencilDepthFailOp = convertStencilOp(opState.depthFailOp),
			.StencilPassOp		= convertStencilOp(opState.passOp),
			.StencilFunc		= convertComparisonOp(opState.compareOp),
		};
	}

	constexpr D3D12_LOGIC_OP convertLogicOp(LogicOperation op)
	{
		using enum LogicOperation;
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

	constexpr D3D12_BLEND_OP convertBlendOp(BlendOperation op)
	{
		using enum BlendOperation;
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

	constexpr D3D12_RENDER_TARGET_BLEND_DESC convertColorAttachmentBlendState(BlendState const& state, size_t index)
	{
		return {
			.BlendEnable		   = state.attachmentStates[index].enableBlend,
			.LogicOpEnable		   = state.enableLogicOp,
			.SrcBlend			   = convertBlendFactor(state.attachmentStates[index].srcColorFactor),
			.DestBlend			   = convertBlendFactor(state.attachmentStates[index].dstColorFactor),
			.BlendOp			   = convertBlendOp(state.attachmentStates[index].colorOp),
			.SrcBlendAlpha		   = convertBlendFactor(state.attachmentStates[index].srcAlphaFactor),
			.DestBlendAlpha		   = convertBlendFactor(state.attachmentStates[index].dstAlphaFactor),
			.BlendOpAlpha		   = convertBlendOp(state.attachmentStates[index].alphaOp),
			.LogicOp			   = convertLogicOp(state.logicOp),
			.RenderTargetWriteMask = static_cast<UINT8>(state.attachmentStates[index].writeMask),
		};
	}

	export class Device final : public DeviceBase
	{
	public:
		Device(vt::Driver& driver, vt::Adapter adapter) :
			device(makeDevice(driver, adapter)),
			renderQueue(device.get(), D3D12_COMMAND_LIST_TYPE_DIRECT),
			computeQueue(device.get(), D3D12_COMMAND_LIST_TYPE_COMPUTE),
			copyQueue(device.get(), D3D12_COMMAND_LIST_TYPE_COPY)
		{}

		DeviceHandle handle() override
		{
			return {device.get()};
		}

		QueueHandle presentationQueueHandle() override
		{
			return {renderQueue.handle()};
		}

		DeferredUnique<PipelineHandle> makeRenderPipeline(RenderPipelineDescription const& desc) override
		{
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[RenderPipelineDescription::MaxVertexAttributes];
			for(unsigned i = 0; i < desc.vertexAttributeCount; ++i)
				inputElementDescs[i] = convertVertexAttribute(desc.vertexAttributes[i]);

			D3D12_GRAPHICS_PIPELINE_STATE_DESC const d3d12Desc {
				.pRootSignature = desc.rootSignature.d3d12(),
				.VS {
					.pShaderBytecode = desc.vertexShaderBytecode.data(),
					.BytecodeLength	 = desc.vertexShaderBytecode.size(),
				},
				.PS {
					.pShaderBytecode = desc.fragmentShaderBytecode.data(),
					.BytecodeLength	 = desc.fragmentShaderBytecode.size(),
				},
				.BlendState {
					.AlphaToCoverageEnable	= desc.multisample.enableAlphaToCoverage,
					.IndependentBlendEnable = true,
					.RenderTarget {
						convertColorAttachmentBlendState(desc.blend, 0),
						convertColorAttachmentBlendState(desc.blend, 1),
						convertColorAttachmentBlendState(desc.blend, 2),
						convertColorAttachmentBlendState(desc.blend, 3),
						convertColorAttachmentBlendState(desc.blend, 4),
						convertColorAttachmentBlendState(desc.blend, 5),
						convertColorAttachmentBlendState(desc.blend, 6),
						convertColorAttachmentBlendState(desc.blend, 7),
					},
				},
				.SampleMask = desc.multisample.sampleMask,
				.RasterizerState {
					.FillMode			   = convertFillMode(desc.rasterizer.fillMode),
					.CullMode			   = convertCullMode(desc.rasterizer.cullMode),
					.FrontCounterClockwise = desc.rasterizer.frontFace == FrontFace::CounterClockwise,
					.DepthBias			   = desc.rasterizer.depthBias,
					.DepthBiasClamp		   = desc.rasterizer.depthBiasClamp,
					.SlopeScaledDepthBias  = desc.rasterizer.depthBiasSlope,
					.DepthClipEnable	   = desc.rasterizer.enableDepthClip,
					.ForcedSampleCount	   = desc.multisample.rasterizerSampleCount,
				},
				.DepthStencilState {
					.DepthEnable	  = desc.depthStencil.enableDepthTest,
					.DepthWriteMask	  = desc.depthStencil.enableDepthWrite ? D3D12_DEPTH_WRITE_MASK_ALL
																		   : D3D12_DEPTH_WRITE_MASK_ZERO,
					.DepthFunc		  = convertComparisonOp(desc.depthStencil.depthComparisonOp),
					.StencilEnable	  = desc.depthStencil.enableStencilTest,
					.StencilReadMask  = desc.depthStencil.stencilReadMask,
					.StencilWriteMask = desc.depthStencil.stencilWriteMask,
					.FrontFace		  = convertStencilOpState(desc.depthStencil.front),
					.BackFace		  = convertStencilOpState(desc.depthStencil.back),
				},
				.InputLayout {
					.pInputElementDescs = inputElementDescs,
					.NumElements		= desc.vertexAttributeCount,
				},
				.IBStripCutValue	   = desc.enablePrimitiveRestart ? D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF
																	 : D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
				.PrimitiveTopologyType = categorizePrimitiveTopology(desc.primitiveTopology),
				.NumRenderTargets	   = desc.blend.attachmentCount,
				.SampleDesc			   = {.Count = desc.multisample.sampleCount},
			};

			ID3D12PipelineState* pipeline;

			auto result = device->CreateGraphicsPipelineState(&d3d12Desc, IID_PPV_ARGS(&pipeline));
			vtAssertResult(result, "Failed to create render pipeline.");

			return DeferredUnique<PipelineHandle>({pipeline});
		}

		void submitCopyCommands(std::span<CommandListHandle> commandLists) override
		{
#if VT_DEBUG
			for(auto list : commandLists)
			{
				bool isRightType = list.d3d12()->GetType() == D3D12_COMMAND_LIST_TYPE_COPY;
				vtAssert(isRightType, "All command lists for this submission must be copy command lists.");
			}
#endif

			copyQueue.submit(commandLists);
		}

		void submitComputeCommands(std::span<CommandListHandle> commandLists) override
		{
#if VT_DEBUG
			for(auto list : commandLists)
			{
				bool isRightType = list.d3d12()->GetType() == D3D12_COMMAND_LIST_TYPE_COMPUTE;
				vtAssert(isRightType, "All command lists for this submission must be compute command lists.");
			}
#endif

			computeQueue.submit(commandLists);
		}

		void submitRenderCommands(std::span<CommandListHandle> commandLists) override
		{
#if VT_DEBUG
			for(auto list : commandLists)
			{
				bool isRightType = list.d3d12()->GetType() == D3D12_COMMAND_LIST_TYPE_DIRECT;
				vtAssert(isRightType, "All command lists for this submission must be render command lists.");
			}
#endif

			renderQueue.submit(commandLists);
		}

	private:
		UniqueInterface<ID3D12Device1> device;
		Queue						   renderQueue;
		Queue						   computeQueue;
		Queue						   copyQueue;

		static ID3D12Device1* makeDevice(vt::Driver& driver, vt::Adapter& adapter)
		{
			auto d3d12CreateDevice = driver.d3d12.deviceCreationFunction();

			ID3D12Device1* device;
			vtEnsureResult(d3d12CreateDevice(adapter.d3d12.handle(), Driver::FeatureLevel, IID_PPV_ARGS(&device)),
						   "Failed to create D3D12 device.");
			return device;
		}
	};
}
