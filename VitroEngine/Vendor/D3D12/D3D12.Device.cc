module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <ranges>
#include <span>
export module Vitro.D3D12.Device;

import Vitro.D3D12.Pipeline;
import Vitro.D3D12.Queue;
import Vitro.D3D12.Texture;
import Vitro.D3D12.Utils;
import Vitro.Graphics.Adapter;
import Vitro.Graphics.DeferredUnique;
import Vitro.Graphics.DeviceBase;
import Vitro.Graphics.Driver;
import Vitro.Graphics.Handle;
import Vitro.Graphics.PipelineInfo;
import Vitro.Graphics.TextureInfo;

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

		DeviceHandle handle() override
		{
			return {device.get()};
		}

		QueueHandle presentationQueueHandle() override
		{
			return {renderQueue.handle()};
		}

		DeferredUnique<PipelineHandle> makeRenderPipeline(RenderPipelineInfo const& info) override
		{
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[RenderPipelineInfo::MaxVertexAttributes];
			for(unsigned i = 0; i < info.vertexAttributeCount; ++i)
				inputElementDescs[i] = convertVertexAttribute(info.vertexAttributes[i]);

			D3D12_GRAPHICS_PIPELINE_STATE_DESC const desc {
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
					.RenderTarget {
						convertColorAttachmentBlendState(info.blend, 0),
						convertColorAttachmentBlendState(info.blend, 1),
						convertColorAttachmentBlendState(info.blend, 2),
						convertColorAttachmentBlendState(info.blend, 3),
						convertColorAttachmentBlendState(info.blend, 4),
						convertColorAttachmentBlendState(info.blend, 5),
						convertColorAttachmentBlendState(info.blend, 6),
						convertColorAttachmentBlendState(info.blend, 7),
					},
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
					.pInputElementDescs = inputElementDescs,
					.NumElements		= info.vertexAttributeCount,
				},
				.IBStripCutValue	   = info.enablePrimitiveRestart ? D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF
																	 : D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED,
				.PrimitiveTopologyType = categorizePrimitiveTopology(info.primitiveTopology),
				.NumRenderTargets	   = info.blend.attachmentCount,
				.RTVFormats			   =,
				.DSVFormat			   =,
				.SampleDesc			   = {.Count = info.multisample.sampleCount},
			};

			ID3D12PipelineState* pipeline;

			auto result = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipeline));
			vtAssertResult(result, "Failed to create render pipeline.");

			return DeferredUnique<PipelineHandle>({pipeline});
		}

		DeferredUnique<TextureHandle> makeTexture(TextureInfo const&) override
		{
			return DeferredUnique<TextureHandle>({});
		}

		void submitCopyCommands(std::span<CommandListHandle> commandLists) override
		{
#if VT_DEBUG
			for(auto list : commandLists)
			{
				bool isRightType = list.d3d12()->GetType() == D3D12_COMMAND_LIST_TYPE_COPY;
				vtAssertPure(isRightType, "All command lists for this submission must be copy command lists.");
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
				vtAssertPure(isRightType, "All command lists for this submission must be compute command lists.");
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
				vtAssertPure(isRightType, "All command lists for this submission must be render command lists.");
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
