export module Vitro.Graphics.GraphicsSystem;

import Vitro.Graphics.DriverContext;
import Vitro.Graphics.DynamicGraphicsAPI;

export class GraphicsSystem
{
	friend class Engine;

private:
	DynamicGraphicsAPI dynamicApi;
	DriverContext driverContext;

	GraphicsSystem() = default;
};
