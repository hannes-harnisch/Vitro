export module Vitro.Graphics.GraphicsSystem;

import Vitro.Graphics.Driver;
import Vitro.Graphics.DynamicGraphicsAPI;

export class GraphicsSystem
{
	friend class Engine;

private:
	DynamicGraphicsAPI dynamicApi;
	Driver driver;

	GraphicsSystem() = default;
};
