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
			case QueuePurpose::Graphics: return D3D12_COMMAND_LIST_TYPE_DIRECT;
		}
	}

	template<QueuePurpose Purpose> class CommandListData
	{};

	template<> class CommandListData<QueuePurpose::Graphics>
	{
	protected:
	};

	export template<QueuePurpose Purpose>
	class CommandList final : public GraphicsCommandListBase, public CommandListData<Purpose>
	{
	public:
		CommandList(vt::Device const& device) : allocator(makeAllocator(device)), commands(makeCommandList(device))
		{}

		void begin() override
		{
			auto result = allocator->Reset();
			vtAssertResult(result, "Failed to reset D3D12 command allocator.");
		}

		void end() override
		{
			auto result = commands->Close();
			vtAssertResult(result, "Failed to end D3D12 command list.");
		}

		void bindPipeline(vt::PipelineHandle const pipeline) override
		{
			commands->SetPipelineState(pipeline.d3d12.handle);
		}

		void bindViewport(Rectangle const viewport) override
		{
			D3D12_VIEWPORT const rect {
				.Width	  = static_cast<FLOAT>(viewport.width),
				.Height	  = static_cast<FLOAT>(viewport.height),
				.MaxDepth = D3D12_MAX_DEPTH,
			};
			commands->RSSetViewports(1, &rect);
		}

		void bindScissor(Rectangle const scissor) override
		{
			D3D12_RECT const rect {
				.right	= static_cast<LONG>(scissor.width),
				.bottom = static_cast<LONG>(scissor.height),
			};
			commands->RSSetScissorRects(1, &rect);
		}

		void beginRenderPass()
		{}

		void transitionToNextSubpass() override
		{}

		void endRenderPass() override
		{
			commands->EndRenderPass();
		}

	private:
		constexpr static D3D12_COMMAND_LIST_TYPE Type = mapQueuePurposeToCommandListType(Purpose);

		ComUnique<ID3D12CommandAllocator> allocator;
		ComUnique<ID3D12GraphicsCommandList4> commands;

		static ComUnique<ID3D12CommandAllocator> makeAllocator(vt::Device const& device)
		{
			ComUnique<ID3D12CommandAllocator> allocator;
			auto result = device.d3d12.getDevice()->CreateCommandAllocator(Type, IID_PPV_ARGS(&allocator));
			vtAssertResult(result, "Failed to create D3D12 command allocator.");
			return allocator;
		}

		ComUnique<ID3D12GraphicsCommandList4> makeCommandList(vt::Device const& device)
		{
			ComUnique<ID3D12GraphicsCommandList4> list;
			auto result = device.d3d12.getDevice()->CreateCommandList(0, Type, allocator, nullptr, IID_PPV_ARGS(&list));
			vtAssertResult(result, "Failed to create D3D12 command list.");
			return list;
		}
	};
}
