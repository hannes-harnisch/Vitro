export module Vitro.Graphics.Pipeline;

import Vitro.Core.Unique;
import Vitro.D3D12.Pipeline;
import Vitro.Graphics.Handle;

export using PipelineHandle = Handle<D3D12::Pipeline, D3D12::Pipeline>;

export class Pipeline
{
public:
private:
	Unique<PipelineHandle, &PipelineHandle::destroy> handle;
};
