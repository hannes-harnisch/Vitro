module;
#include "Direct3D.API.hh"
export module Vitro.Direct3D.CommandList;

import Vitro.Direct3D.Unique;
import Vitro.Graphics.CommandListBase;
import Vitro.Graphics.Pipeline;

namespace Direct3D
{
	export class CommandList final : public CommandListBase
	{
	public:
		void bindPipeline(PipelineHandle pipeline) override
		{
			commandList->SetPipelineState(pipeline.d3d.handle);
		}

	private:
		Unique<ID3D12GraphicsCommandList4> commandList;
	};
}
