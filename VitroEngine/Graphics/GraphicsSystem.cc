module;
#include <unordered_map>
export module Vitro.Graphics.GraphicsSystem;

import Vitro.Graphics.Driver;
import Vitro.Graphics.DynamicGraphicsAPI;
import Vitro.Graphics.SwapChain;

export class GraphicsSystem
{
public:
private:
	DynamicGraphicsAPI dynamicApi;
	Driver driver;
	std::unordered_map<Window*, SwapChain> swapChains;
};
