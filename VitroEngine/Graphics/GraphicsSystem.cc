module;
#include "Core/Macros.hh"

#include <algorithm>
#include <unordered_map>
export module vt.Graphics.GraphicsSystem;

import vt.App.EventListener;
import vt.App.ObjectEvent;
import vt.App.Window;
import vt.App.WindowEvent;
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
	export class GraphicsSystem : public EventListener
	{
	public:
		GraphicsSystem() :
			device(driver, select_adapter()), present_pass(device, fill_present_pass_info()), renderer(device, present_pass)
		{
			register_event_handlers<&GraphicsSystem::on_paint, &GraphicsSystem::on_window_resize,
									&GraphicsSystem::on_window_object_construct,
									&GraphicsSystem::on_window_object_move_construct, &GraphicsSystem::on_window_object_destroy,
									&GraphicsSystem::on_window_object_move_assign>();
		}

	private:
		DynamicGpuApi								 dynamic_gpu_api;
		Driver										 driver;
		Device										 device;
		std::unordered_map<Window const*, SwapChain> swap_chains;
		RenderPass									 present_pass;
		ForwardRenderer								 renderer;

		static RenderPassInfo fill_present_pass_info()
		{
			Subpass subpass {{
				AttachmentReference {
					.used_layout = ImageLayout::ColorAttachment,
				},
			}};
			return RenderPassInfo {
				.attachments {
					AttachmentInfo {
						.format			= ImageFormat::R8_G8_B8_A8_UNorm,
						.load_op		= ImageLoadOp::Clear,
						.store_op		= ImageStoreOp::Store,
						.initial_layout = ImageLayout::Undefined,
						.final_layout	= ImageLayout::Presentable,
					},
				},
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

		void on_paint(WindowPaintEvent& event)
		{
			auto& swap_chain = swap_chains.at(&event.window);
			renderer.draw(swap_chain);
		}

		void on_window_resize(WindowSizeEvent& event)
		{
			swap_chains.at(&event.window)->resize(event);
		}

		void on_window_object_construct(ObjectConstructEvent<Window>& event)
		{
			emplace_swap_chain(event.object);
		}

		void on_window_object_move_construct(ObjectMoveConstructEvent<Window>& event)
		{
			replace_key_to_swap_chain(event.moved, event.constructed);
		}

		void on_window_object_destroy(ObjectDestroyEvent<Window>& event)
		{
			remove_swap_chain(event.object);
		}

		void on_window_object_move_assign(ObjectMoveAssignEvent<Window>& event)
		{
			remove_swap_chain(event.left);
			replace_key_to_swap_chain(event.right, event.left);
		}

		void emplace_swap_chain(Window& window)
		{
			swap_chains.try_emplace(&window, driver, device, present_pass, window.native_handle());
		}

		void remove_swap_chain(Window const& window)
		{
			device->flush_render_queue();
			swap_chains.erase(&window);
		}

		void replace_key_to_swap_chain(Window const& old_window, Window const& new_window)
		{
			auto node  = swap_chains.extract(&old_window);
			node.key() = &new_window;
			swap_chains.insert(std::move(node));
		}
	};
}
