module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"
export module Vitro.D3D12.CommandList;

import Vitro.D3D12.Unique;
import Vitro.Graphics.CommandListBase;
import Vitro.Graphics.Handle;

namespace D3D12
{
	export class CommandList final : public CommandListBase
	{
	public:
		void begin() override
		{
			auto result = allocator->Reset();
			vtAssertResult(result, "Failed to reset D3D12 command allocator.");
		}

		void bindPipeline(PipelineHandle const pipeline) override
		{
			commands->SetPipelineState(pipeline.d3d12.handle);
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
