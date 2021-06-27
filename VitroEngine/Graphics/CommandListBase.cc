export module Vitro.Graphics.CommandListBase;

import Vitro.Graphics.Handle;

export class CommandListBase
{
public:
	virtual void begin()					  = 0;
	virtual void bindPipeline(PipelineHandle) = 0;
	virtual void end()						  = 0;
};
