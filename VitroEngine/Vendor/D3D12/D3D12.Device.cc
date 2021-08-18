module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <ranges>
#include <span>
export module Vitro.D3D12.Device;

import Vitro.Core.Algorithm;
import Vitro.Core.FixedList;
import Vitro.D3D12.Pipeline;
import Vitro.D3D12.Queue;
import Vitro.D3D12.Texture;
import Vitro.D3D12.Utils;
import Vitro.Graphics.Adapter;
import Vitro.Graphics.DeferredUnique;
import Vitro.Graphics.DeviceBase;
import Vitro.Graphics.Driver;
import Vitro.Graphics.FrameContext;
import Vitro.Graphics.Handle;
import Vitro.Graphics.PipelineInfo;
import Vitro.Graphics.TextureInfo;
import Vitro.Trace.Log;

namespace vt::d3d12
{
	export class Device final : public DeviceBase
	{
	public:
		Device(vt::Driver& driver, vt::Adapter adapter) :
			device(makeDevice(driver, adapter)),
			renderQueue(device.get(), D3D12_COMMAND_LIST_TYPE_DIRECT),
			computeQueue(device.get(), D3D12_COMMAND_LIST_TYPE_COMPUTE),
			copyQueue(device.get(), D3D12_COMMAND_LIST_TYPE_COPY)
		{}

		DeferredUnique<PipelineHandle> makeRenderPipeline(RenderPipelineInfo const& info) override
		{
			FixedList<D3D12_INPUT_ELEMENT_DESC, MaxVertexAttributes> inputElementDescs;
			for(auto attrib : info.vertexAttributes)
				inputElementDescs.emplace_back(convertVertexAttribute(attrib));

			auto& pass = info.renderPass.d3d12;

			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc {
				.pRootSignature = info.rootSignature.d3d12(),
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
					.IndependentBlendEnable = true,
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
					.DepthFunc		  = convertComparisonOp(info.depthStencil.depthComparisonOp),
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

			ID3D12PipelineState* pipeline;

			auto result = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipeline));
			vtAssertResult(result, "Failed to create render pipeline.");

			return DeferredUnique<PipelineHandle>({pipeline});
		}

		DeferredUnique<TextureHandle> makeTexture(TextureInfo const&) override
		{
			return DeferredUnique<TextureHandle>({});
		}

		void submitRenderCommands(std::span<CommandListHandle> commandLists, unsigned frameIndex) override
		{
			validateCommandLists<D3D12_COMMAND_LIST_TYPE_DIRECT>(commandLists);

			renderQueue.submit(commandLists);
			renderFenceValues[frameIndex] = renderQueue.signal();
		}

		void submitComputeCommands(std::span<CommandListHandle> commandLists) override
		{
			validateCommandLists<D3D12_COMMAND_LIST_TYPE_COMPUTE>(commandLists);

			computeQueue.submit(commandLists);
			computeQueue.signal();
		}

		void submitCopyCommands(std::span<CommandListHandle> commandLists) override
		{
			validateCommandLists<D3D12_COMMAND_LIST_TYPE_COPY>(commandLists);

			copyQueue.submit(commandLists);
			copyQueue.signal();
		}

		void awaitFenceValueByIndex(unsigned frameIndex)
		{
			renderQueue.awaitFenceValue(frameIndex);
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
		ComUnique<ID3D12Device1> device;
		Queue					 renderQueue;
		Queue					 computeQueue;
		Queue					 copyQueue;
		uint64_t				 renderFenceValues[MaxFramesInFlight] = {};

		static ID3D12Device1* makeDevice(vt::Driver& driver, vt::Adapter& adapter)
		{
			auto d3d12CreateDevice = driver.d3d12.deviceCreationFunction();

			ID3D12Device1* device;

			auto result = d3d12CreateDevice(adapter.d3d12.handle(), Driver::FeatureLevel, IID_PPV_ARGS(&device));
			vtEnsureResult(result, "Failed to create D3D12 device.");

			return device;
		}

		template<D3D12_COMMAND_LIST_TYPE Type> static void validateCommandLists(std::span<CommandListHandle> commandLists)
		{
#if VT_DEBUG
			for(auto list : commandLists)
			{
				bool isRightType = list.d3d12()->GetType() == Type;
				vtAssert(isRightType, "The type of this command list does not match the type of queue it was submitted to.");
			}
#endif
		}
	};
}
