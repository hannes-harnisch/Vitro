module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"

#include <span>
export module Vitro.D3D12.Device;

import Vitro.D3D12.Queue;
import Vitro.D3D12.Utils;
import Vitro.Graphics.Adapter;
import Vitro.Graphics.DeferredUnique;
import Vitro.Graphics.DeviceBase;
import Vitro.Graphics.Driver;
import Vitro.Graphics.Handle;
import Vitro.Graphics.PipelineState;

namespace vt::d3d12
{
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
		return D3D12_DEPTH_STENCILOP_DESC {
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
		return D3D12_RENDER_TARGET_BLEND_DESC {
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

		DeferredUnique<PipelineHandle> makePipeline(PipelineState const& state) override
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC const desc {
				.pRootSignature = state.rootSignature.d3d12(),
				.VS {
					.pShaderBytecode = state.vertexShaderBytecode.data(),
					.BytecodeLength	 = state.vertexShaderBytecode.size(),
				},
				.PS {
					.pShaderBytecode = state.fragmentShaderBytecode.data(),
					.BytecodeLength	 = state.fragmentShaderBytecode.size(),
				},
				.BlendState {
					.AlphaToCoverageEnable	= state.multisample.enableAlphaToCoverage,
					.IndependentBlendEnable = true,
					.RenderTarget {
						convertColorAttachmentBlendState(state.blend, 0),
						convertColorAttachmentBlendState(state.blend, 1),
						convertColorAttachmentBlendState(state.blend, 2),
						convertColorAttachmentBlendState(state.blend, 3),
						convertColorAttachmentBlendState(state.blend, 4),
						convertColorAttachmentBlendState(state.blend, 5),
						convertColorAttachmentBlendState(state.blend, 6),
						convertColorAttachmentBlendState(state.blend, 7),
					},
				},
				.SampleMask = state.multisample.sampleMask,
				.DepthStencilState {
					.DepthEnable	  = state.depthStencil.enableDepthTest,
					.DepthWriteMask	  = static_cast<D3D12_DEPTH_WRITE_MASK>(state.depthStencil.enableDepthWrite),
					.DepthFunc		  = convertComparisonOp(state.depthStencil.depthComparisonOp),
					.StencilEnable	  = state.depthStencil.enableStencilTest,
					.StencilReadMask  = state.depthStencil.stencilReadMask,
					.StencilWriteMask = state.depthStencil.stencilWriteMask,
					.FrontFace		  = convertStencilOpState(state.depthStencil.front),
					.BackFace		  = convertStencilOpState(state.depthStencil.back),
				},
			};
			ID3D12PipelineState* pipeline;
			auto result = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipeline));
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

		ID3D12Device1* handle()
		{
			return device.get();
		}

		ID3D12CommandQueue* renderQueueHandle()
		{
			return renderQueue.handle();
		}

		ID3D12CommandQueue* computeQueueHandle()
		{
			return computeQueue.handle();
		}

		ID3D12CommandQueue* copyQueueHandle()
		{
			return copyQueue.handle();
		}

	private:
		UniqueInterface<ID3D12Device1> device;
		Queue renderQueue;
		Queue computeQueue;
		Queue copyQueue;

		static ID3D12Device1* makeDevice(vt::Driver& driver, vt::Adapter& adapter)
		{
			auto d3d12CreateDevice = driver.d3d12.deviceCreationFunction();
			ID3D12Device1* device;
			auto result = d3d12CreateDevice(adapter.d3d12.handle(), Driver::FeatureLevel, IID_PPV_ARGS(&device));
			vtEnsureResult(result, "Failed to create D3D12 device.");
			return device;
		}
	};
}
