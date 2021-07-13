module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"
export module Vitro.D3D12.CommandList;

import Vitro.D3D12.ComUnique;
import Vitro.Graphics.CommandListBase;
import Vitro.Graphics.Device;
import Vitro.Graphics.Handle;
import Vitro.Math.Rectangle;

namespace vt::d3d12
{
	consteval D3D12_COMMAND_LIST_TYPE mapQueuePurposeToCommandListType(QueuePurpose purpose)
	{
		switch(purpose)
		{
			case QueuePurpose::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
			case QueuePurpose::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
			case QueuePurpose::Render: return D3D12_COMMAND_LIST_TYPE_DIRECT;
		}
	}

	template<QueuePurpose Purpose> class CommandListData
	{};

	template<> class CommandListData<QueuePurpose::Compute> : public CommandListData<QueuePurpose::Copy>
	{};

	template<> class CommandListData<QueuePurpose::Render> : public CommandListData<QueuePurpose::Compute>
	{
	protected:
		RenderPassHandle currentRenderPass;
	};

	export template<QueuePurpose Purpose>
	class CommandList final : public RenderCommandListBase, public CommandListData<Purpose>
	{
	public:
		CommandList(vt::Device const& device) : allocator(makeAllocator(device)), cmd(makeCommandList(device))
		{}

		void begin() override
		{
			auto result = allocator->Reset();
			vtAssertResult(result, "Failed to reset D3D12 command allocator.");
		}

		void end() override
		{
			auto result = cmd->Close();
			vtAssertResult(result, "Failed to end D3D12 command list.");
		}

		void bindPipeline(vt::PipelineHandle const pipeline) override
		{
			cmd->SetPipelineState(pipeline.d3d12.handle);
		}

		void bindRootSignature(vt::RootSignatureHandle const rootSignature) override
		{
			if constexpr(Purpose == QueuePurpose::Compute)
				cmd->SetComputeRootSignature(rootSignature.d3d12.handle);
			else
				cmd->SetGraphicsRootSignature(rootSignature.d3d12.handle);
		}

		void bindViewport(Rectangle const viewport) override
		{
			D3D12_VIEWPORT const rect {
				.Width	  = static_cast<FLOAT>(viewport.width),
				.Height	  = static_cast<FLOAT>(viewport.height),
				.MaxDepth = D3D12_MAX_DEPTH,
			};
			cmd->RSSetViewports(1, &rect);
		}

		void bindScissor(Rectangle const scissor) override
		{
			D3D12_RECT const rect {
				.right	= static_cast<LONG>(scissor.width),
				.bottom = static_cast<LONG>(scissor.height),
			};
			cmd->RSSetScissorRects(1, &rect);
		}

		void beginRenderPass(vt::RenderPassHandle renderPass, vt::RenderTargetHandle renderTarget) override
		{
			D3D12_RESOURCE_BARRIER const barrier {
				.Transition =
					{
						.pResource	 = renderTarget.d3d12.handle,
						.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
						.StateBefore = D3D12_RESOURCE_STATE_COMMON,
						.StateAfter	 = D3D12_RESOURCE_STATE_RENDER_TARGET,
					},
			};
			cmd->ResourceBarrier(1, &barrier);
			cmd->OMSetRenderTargets(1, , true, );
			cmd->BeginRenderPass();
		}

		void transitionToNextSubpass() override
		{}

		void endRenderPass() override
		{
			cmd->EndRenderPass();
		}

	private:
		constexpr static D3D12_COMMAND_LIST_TYPE Type = mapQueuePurposeToCommandListType(Purpose);

		ComUnique<ID3D12CommandAllocator> allocator;
		ComUnique<ID3D12GraphicsCommandList4> cmd;

		static ComUnique<ID3D12CommandAllocator> makeAllocator(vt::Device const& device)
		{
			ComUnique<ID3D12CommandAllocator> allocator;
			auto result = device.d3d12.handle()->CreateCommandAllocator(Type, IID_PPV_ARGS(&allocator));
			vtAssertResult(result, "Failed to create D3D12 command allocator.");
			return allocator;
		}

		ComUnique<ID3D12GraphicsCommandList4> makeCommandList(vt::Device const& device)
		{
			ComUnique<ID3D12GraphicsCommandList4> list;
			auto result = device.d3d12.handle()->CreateCommandList(0, Type, allocator, nullptr, IID_PPV_ARGS(&list));
			vtAssertResult(result, "Failed to create D3D12 command list.");
			return list;
		}
	};
}
