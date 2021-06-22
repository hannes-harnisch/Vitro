export module Vitro.Graphics.GraphicsSystem;

import Vitro.Graphics.Driver;
import Vitro.Graphics.DynamicGraphicsAPI;
import Vitro.Trace.Log;

export class GraphicsSystem
{
	friend class Engine;

private:
	DynamicGraphicsAPI dynamicApi;
	Driver driver;

	GraphicsSystem() = default;
};
