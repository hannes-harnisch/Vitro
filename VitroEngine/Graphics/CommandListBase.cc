export module Vitro.Graphics.CommandListBase;

import Vitro.Graphics.PipelineHandle;

export class CommandListBase
{
public:
	virtual void bindPipeline(PipelineHandle pipeline) = 0;
};
