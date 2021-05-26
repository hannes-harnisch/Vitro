export module Vitro.Graphics.GraphicsSystem;

import Vitro.Graphics.GraphicsContext;

export class GraphicsSystem
{
	friend class Engine;

private:
	GraphicsContext graphicsContext;

	GraphicsSystem() = default;
};
