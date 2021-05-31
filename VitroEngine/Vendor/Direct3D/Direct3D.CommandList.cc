module;
#include "Direct3D.API.hh"
export module Vitro.Direct3D.CommandList;

import Vitro.Direct3D.Unique;
import Vitro.Graphics.Pipeline;

namespace Direct3D
{
	export class CommandList
	{
	public:
		void bindPipeline(Pipeline const& pipeline)
		{
			// commandList->SetPipelineState(pipeline.handle());
		}

	private:
		Unique<ID3D12GraphicsCommandList> commandList;
	};
}
