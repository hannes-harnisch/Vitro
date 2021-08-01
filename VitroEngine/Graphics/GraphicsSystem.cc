module;
#include "Trace/Assert.hh"

#include <algorithm>
export module Vitro.Graphics.GraphicsSystem;

import Vitro.Core.HashMap;
import Vitro.Core.Singleton;
import Vitro.Graphics.Adapter;
import Vitro.Graphics.Device;
import Vitro.Graphics.Driver;
import Vitro.Graphics.DynamicGpuApi;
import Vitro.Graphics.ForwardRenderer;
import Vitro.Graphics.SwapChain;

namespace vt
{
	export class GraphicsSystem : public Singleton<GraphicsSystem>
	{
	public:
		void notifyWindowConstruction(class Window const& window, void* nativeWindowHandle)
		{
			swapChains.try_emplace(&window, device->handle(), device->presentationQueueHandle(), nativeWindowHandle);
		}

		void notifyWindowReplacement(Window const& oldWindow, Window const& newWindow)
		{
			auto node  = swapChains.extract(&oldWindow);
			node.key() = &newWindow;
			swapChains.insert(std::move(node));
		}

		void notifyWindowDestruction(Window const& window)
		{
			swapChains.erase(&window);
		}

		GraphicsSystem() : device(driver, selectAdapter()), renderer(device)
		{}

		void update()
		{
			for(auto& [window, swapChain] : swapChains)
				renderer.draw(swapChain);
		}

	private:
		DynamicGpuApi					  dynamicGpuApi;
		Driver							  driver;
		Device							  device;
		HashMap<Window const*, SwapChain> swapChains;
		ForwardRenderer					  renderer;

		Adapter selectAdapter()
		{
			auto adapters = driver->enumerateAdapters();
			auto selected = std::max_element(adapters.begin(), adapters.end(), [](Adapter const& a1, Adapter const& a2) {
				return a1->getVRAM() < a2->getVRAM();
			});
			vtEnsure(selected != adapters.end(), "No suitable GPUs found.");
			return std::move(*selected);
		}
	};
}
