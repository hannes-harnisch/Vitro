module;
#include "Core/Macros.hh"

#include <algorithm>
export module vt.Graphics.GraphicsSystem;

import vt.App.EventListener;
import vt.App.WindowEvent;
import vt.Core.HashMap;
import vt.Core.Singleton;
import vt.Graphics.Adapter;
import vt.Graphics.Device;
import vt.Graphics.Driver;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.ForwardRenderer;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderPassInfo;
import vt.Graphics.SwapChain;
import vt.Graphics.TextureInfo;

namespace vt
{
	export class GraphicsSystem : public Singleton<GraphicsSystem>, public EventListener
	{
	public:
		GraphicsSystem() :
			device(driver, select_adapter()), present_pass(device, fill_present_pass_info()), renderer(device, present_pass)
		{
			register_event_handlers<&GraphicsSystem::update>();
		}

		void notify_window_construction(class Window const& window, void* native_window_handle)
		{
			swap_chains.try_emplace(&window, driver, device, present_pass, native_window_handle);
		}

		void notify_window_replacement(Window const& old_window, Window const& new_window)
		{
			auto node  = swap_chains.extract(&old_window);
			node.key() = &new_window;
			swap_chains.insert(std::move(node));
		}

		void notify_window_destruction(Window const& window)
		{
			device->flush_render_queue();
			swap_chains.erase(&window);
		}

	private:
		DynamicGpuApi					  dynamic_gpu_api;
		Driver							  driver;
		Device							  device;
		HashMap<Window const*, SwapChain> swap_chains;
		RenderPass						  present_pass;
		ForwardRenderer					  renderer;

		static RenderPassInfo fill_present_pass_info()
		{
			Subpass subpass {{AttachmentReference {.used_layout = ImageLayout::ColorAttachment}}};
			return RenderPassInfo {
				.attachments {AttachmentInfo {
					.format			= ImageFormat::R8_G8_B8_A8_UNorm,
					.load_op		= ImageLoadOp::Clear,
					.store_op		= ImageStoreOp::Store,
					.initial_layout = ImageLayout::Undefined,
					.final_layout	= ImageLayout::Presentable,
				}},
				.subpasses {subpass},
			};
		}

		Adapter select_adapter()
		{
			auto adapters = driver->enumerate_adapters();
			auto selected = std::max_element(adapters.begin(), adapters.end(), [](Adapter const& a1, Adapter const& a2) {
				return a1.get_vram() < a2.get_vram();
			});
			VT_ENSURE(selected != adapters.end(), "No suitable GPUs found.");
			return std::move(*selected);
		}

		void update(WindowPaintEvent& e)
		{
			auto& swap_chain = swap_chains.at(&e.window);
			renderer.draw(swap_chain);
		}
	};
}
