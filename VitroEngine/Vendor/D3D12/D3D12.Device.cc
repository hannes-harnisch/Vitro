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
	constexpr D3D12_LOGIC_OP convertLogicOp(LogicOperation op)
	{
		switch(op)
		{
			case LogicOperation::Clear: return D3D12_LOGIC_OP_CLEAR;
			case LogicOperation::Set: return D3D12_LOGIC_OP_SET;
			case LogicOperation::Copy: return D3D12_LOGIC_OP_COPY;
			case LogicOperation::CopyInverted: return D3D12_LOGIC_OP_COPY_INVERTED;
			case LogicOperation::NoOp: return D3D12_LOGIC_OP_NOOP;
			case LogicOperation::Invert: return D3D12_LOGIC_OP_INVERT;
			case LogicOperation::And: return D3D12_LOGIC_OP_AND;
			case LogicOperation::Nand: return D3D12_LOGIC_OP_NAND;
			case LogicOperation::Or: return D3D12_LOGIC_OP_OR;
			case LogicOperation::Nor: return D3D12_LOGIC_OP_NOR;
			case LogicOperation::Xor: return D3D12_LOGIC_OP_XOR;
			case LogicOperation::Equivalent: return D3D12_LOGIC_OP_EQUIV;
			case LogicOperation::AndReverse: return D3D12_LOGIC_OP_AND_REVERSE;
			case LogicOperation::AndInverted: return D3D12_LOGIC_OP_AND_INVERTED;
			case LogicOperation::OrReverse: return D3D12_LOGIC_OP_OR_REVERSE;
			case LogicOperation::OrInverted: return D3D12_LOGIC_OP_OR_INVERTED;
		}
		vtUnreachable();
	}

	constexpr D3D12_BLEND convertBlendFactor(BlendFactor factor)
	{
		switch(factor)
		{
			case BlendFactor::Zero: return D3D12_BLEND_ZERO;
			case BlendFactor::One: return D3D12_BLEND_ONE;
			case BlendFactor::SrcColor: return D3D12_BLEND_SRC_COLOR;
			case BlendFactor::SrcColorInv: return D3D12_BLEND_INV_SRC_COLOR;
			case BlendFactor::SrcAlpha: return D3D12_BLEND_SRC_ALPHA;
			case BlendFactor::SrcAlphaInv: return D3D12_BLEND_INV_SRC_ALPHA;
			case BlendFactor::DstAlpha: return D3D12_BLEND_DEST_ALPHA;
			case BlendFactor::DstAlphaInv: return D3D12_BLEND_INV_DEST_ALPHA;
			case BlendFactor::DstColor: return D3D12_BLEND_DEST_COLOR;
			case BlendFactor::DstColorInv: return D3D12_BLEND_INV_DEST_COLOR;
			case BlendFactor::SrcAlphaSaturate: return D3D12_BLEND_SRC_ALPHA_SAT;
			case BlendFactor::Src1Color: return D3D12_BLEND_SRC1_COLOR;
			case BlendFactor::Src1ColorInv: return D3D12_BLEND_INV_SRC1_COLOR;
			case BlendFactor::Src1Alpha: return D3D12_BLEND_SRC1_ALPHA;
			case BlendFactor::Src1AlphaInv: return D3D12_BLEND_INV_SRC1_ALPHA;
		}
		vtUnreachable();
	}

	constexpr D3D12_BLEND_OP convertBlendOp(BlendOperation op)
	{
		switch(op)
		{
			case BlendOperation::Add: return D3D12_BLEND_OP_ADD;
			case BlendOperation::Subtract: return D3D12_BLEND_OP_SUBTRACT;
			case BlendOperation::ReverseSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
			case BlendOperation::Min: return D3D12_BLEND_OP_MIN;
			case BlendOperation::Max: return D3D12_BLEND_OP_MAX;
		}
		vtUnreachable();
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
						{
							.BlendEnable		   = state.blend.attachmentStates[0].enableBlend,
							.LogicOpEnable		   = state.blend.enableLogicOp,
							.SrcBlend			   = convertBlendFactor(state.blend.attachmentStates[0].srcColorFactor),
							.DestBlend			   = convertBlendFactor(state.blend.attachmentStates[0].dstColorFactor),
							.BlendOp			   = convertBlendOp(state.blend.attachmentStates[0].colorOp),
							.SrcBlendAlpha		   = convertBlendFactor(state.blend.attachmentStates[0].srcAlphaFactor),
							.DestBlendAlpha		   = convertBlendFactor(state.blend.attachmentStates[0].dstAlphaFactor),
							.BlendOpAlpha		   = convertBlendOp(state.blend.attachmentStates[0].alphaOp),
							.LogicOp			   = convertLogicOp(state.blend.logicOp),
							.RenderTargetWriteMask = static_cast<UINT8>(state.blend.attachmentStates[0].writeMask),
						},
					},
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
