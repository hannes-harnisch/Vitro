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
import Vitro.Graphics.RenderPassInfo;
import Vitro.Graphics.SwapChain;
import Vitro.Graphics.TextureInfo;

namespace vt
{
	export class GraphicsSystem : public Singleton<GraphicsSystem>, public EventListener
	{
	public:
		GraphicsSystem() :
			device(driver, selectAdapter()), presentPass(device, fillPresentPassInfo()), renderer(device, presentPass)
		{
			registerEventHandlers<&GraphicsSystem::update>();
		}

		void notifyWindowConstruction(class Window const& window, void* nativeWindowHandle)
		{
			swapChains.try_emplace(&window, driver, device, presentPass, nativeWindowHandle);
		}

		void notifyWindowReplacement(Window const& oldWindow, Window const& newWindow)
		{
			auto node  = swapChains.extract(&oldWindow);
			node.key() = &newWindow;
			swapChains.insert(std::move(node));
		}

		void notifyWindowDestruction(Window const& window)
		{
			device->flushRenderQueue();
			swapChains.erase(&window);
		}

	private:
		DynamicGpuApi					  dynamicGpuApi;
		Driver							  driver;
		Device							  device;
		HashMap<Window const*, SwapChain> swapChains;
		RenderPass						  presentPass;
		ForwardRenderer					  renderer;

		static RenderPassInfo fillPresentPassInfo()
		{
			Subpass subpass {{AttachmentReference {.usedLayout = ImageLayout::ColorAttachment}}};
			return RenderPassInfo {
				.attachments {AttachmentInfo {
					.format		   = ImageFormat::R8_G8_B8_A8_UNorm,
					.loadOp		   = ImageLoadOp::Clear,
					.storeOp	   = ImageStoreOp::Store,
					.initialLayout = ImageLayout::Undefined,
					.finalLayout   = ImageLayout::Presentable,
				}},
				.subpasses {subpass},
			};
		}

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
