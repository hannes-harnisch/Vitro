module;
#include "Core/Macros.hh"

#include <algorithm>
export module Vitro.Graphics.GraphicsSystem;

import Vitro.App.EventListener;
import Vitro.App.WindowEvent;
import Vitro.Core.HashMap;
import Vitro.Core.Singleton;
import Vitro.Graphics.Adapter;
import Vitro.Graphics.Device;
import Vitro.Graphics.Driver;
import Vitro.Graphics.DynamicGpuApi;
import Vitro.Graphics.ForwardRenderer;
import Vitro.Graphics.RenderPass;
import Vitro.Graphics.SwapChain;

namespace vt
{
	export class GraphicsSystem : public Singleton<GraphicsSystem>, public EventListener
	{
	public:
		GraphicsSystem() : device(driver, selectAdapter()), renderer(device)
		{
			registerEventHandlers<&GraphicsSystem::update>();
		}

		void notifyWindowConstruction(class Window const& window, void* nativeWindowHandle)
		{
			swapChains.try_emplace(&window, device, nativeWindowHandle);
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

	private:
		DynamicGpuApi					  dynamicGpuApi;
		Driver							  driver;
		Device							  device;
		HashMap<Window const*, SwapChain> swapChains;
		// RenderPass						  presentPass;
		ForwardRenderer renderer;

		Adapter selectAdapter()
		{
			auto adapters = driver->enumerateAdapters();
			auto selected = std::max_element(adapters.begin(), adapters.end(), [](Adapter const& a1, Adapter const& a2) {
				return a1->getVRAM() < a2->getVRAM();
			});
			vtEnsure(selected != adapters.end(), "No suitable GPUs found.");
			return std::move(*selected);
		}

		void update(WindowPaintEvent& e)
		{
			auto& swapChain = swapChains.at(&e.window);
			renderer.draw(swapChain);
		}
	};
}
