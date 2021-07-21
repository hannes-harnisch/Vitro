module;
#include <algorithm>
export module Vitro.Graphics.GraphicsSystem;

import Vitro.Core.HashMap;
import Vitro.Core.Singleton;
import Vitro.Graphics.Adapter;
import Vitro.Graphics.Device;
import Vitro.Graphics.Driver;
import Vitro.Graphics.DynamicGraphicsAPI;
import Vitro.Graphics.ForwardRenderer;
import Vitro.Graphics.SwapChain;

namespace vt
{
	export class GraphicsSystem : public Singleton<GraphicsSystem>
	{
	public:
		GraphicsSystem() : device(driver, selectAdapter()), renderer(device)
		{}

		void update()
		{}

		void notifyWindowConstruction(class Window const& window, void* nativeWindowHandle)
		{
			swapChains.try_emplace(&window, device, nativeWindowHandle, 2);
		}

		void notifyWindowReplacement(Window const& oldWindow, Window const& newWindow)
		{
			auto swapChainNode	= swapChains.extract(&oldWindow);
			swapChainNode.key() = &newWindow;
			swapChains.insert(std::move(swapChainNode));
		}

		void notifyWindowDestruction(Window const& window)
		{
			swapChains.erase(&window);
		}

	private:
		DynamicGraphicsAPI dynamicGraphicsAPI;
		Driver driver;
		Device device;
		HashMap<Window const*, SwapChain> swapChains;
		ForwardRenderer renderer;

		Adapter selectAdapter()
		{
			auto adapters = driver->enumerateAdapters();
			auto selected = std::max_element(adapters.begin(), adapters.end(), [](Adapter const& a1, Adapter const& a2) {
				return a1->getVRAM() < a2->getVRAM();
			});
			return std::move(*selected);
		}
	};
}
