export module Vitro.Graphics.CommandListBase;

import Vitro.Graphics.Pipeline;

export class CommandListBase
{
public:
	virtual void begin()							   = 0;
	virtual void bindPipeline(PipelineHandle pipeline) = 0;
	virtual void end()								   = 0;
};