export module Vitro.Graphics.GraphicsSystem;

import Vitro.Graphics.DriverContext;

export class GraphicsSystem
{
	friend class Engine;

private:
	DriverContext graphicsContext;

	GraphicsSystem() = default;
};
