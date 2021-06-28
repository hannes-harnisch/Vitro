module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"
export module Vitro.D3D12.CommandList;

import Vitro.D3D12.Unique;
import Vitro.Graphics.CommandListBase;
import Vitro.Graphics.Handle;
import Vitro.Math.Rectangle;

namespace vt::d3d12
{
	export class CommandList final : public CommandListBase
	{
	public:
		CommandList()
		{}

		void begin() override
		{
			auto result = allocator->Reset();
			vtAssertResult(result, "Failed to reset D3D12 command allocator.");
		}

		void bindPipeline(PipelineHandle const pipeline) override
		{
			commands->SetPipelineState(pipeline.d3d12.handle);
		}

		void bindViewport(Rectangle const viewport) override
		{
			D3D12_VIEWPORT const rect {
				.Width	  = static_cast<float>(viewport.width),
				.Height	  = static_cast<float>(viewport.height),
				.MaxDepth = D3D12_MAX_DEPTH,
			};
			commands->RSSetViewports(1, &rect);
		}

		void bindScissor(Rectangle const scissor) override
		{
			D3D12_RECT const rect {
				.right	= static_cast<long>(scissor.width),
				.bottom = static_cast<long>(scissor.height),
			};
			commands->RSSetScissorRects(1, &rect);
		}

		void endRenderPass() override
		{
			commands->EndRenderPass();
		}

		void end() override
		{
			auto result = commands->Close();
			vtAssertResult(result, "Failed to end D3D12 command list.");
		}

	private:
		Unique<ID3D12CommandAllocator> allocator;
		Unique<ID3D12GraphicsCommandList4> commands;
	};
}
