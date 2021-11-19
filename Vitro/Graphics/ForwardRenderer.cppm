module;
#include <vector>
export module vt.Graphics.ForwardRenderer;

import vt.App.EventListener;
import vt.App.Input;
import vt.App.WindowEvent;
import vt.Core.Rect;
import vt.Core.Tick;
import vt.Core.Transform;
import vt.Core.Vector;
import vt.Graphics.AssetResource;
import vt.Graphics.Camera;
import vt.Graphics.CommandList;
import vt.Graphics.AbstractCommandList;
import vt.Graphics.DeletionQueue;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.Device;
import vt.Graphics.RendererBase;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTarget;
import vt.Graphics.RingBuffer;
import vt.Graphics.RootSignature;
import vt.Trace.Log;

namespace vt
{
	export class ForwardRenderer : public RendererBase, public EventListener
	{
	public:
		ForwardRenderer(Device& device, Extent shared_render_target_size, ImageFormat shared_render_target_format);

	protected:
		SmallList<CommandListHandle>	render(Tick tick, RenderTarget const& render_target) override;
		SharedRenderTargetSpecification specify_shared_render_target() const override;
		void							on_render_target_resize(Extent size) override;

	private:
		Camera							 cam;
		RenderPass						 final_render_pass;
		std::vector<DescriptorSetLayout> descriptor_set_layouts;
		std::vector<RootSignature>		 root_signatures;
		std::vector<RenderPipeline>		 render_pipelines;
		std::vector<Buffer>				 vertex_buffers;
		std::vector<Buffer>				 index_buffers;
		std::vector<Image>				 depth_images;
		float							 time = 0;

		struct FrameResources
		{
			RenderCommandList render_cmd_list;
			DeletionQueue	  deletion_queue;

			FrameResources(Device& device) : render_cmd_list(device->make_render_command_list())
			{}
		};
		RingBuffer<FrameResources> context;

		RenderPass make_final_render_pass(ImageFormat swap_chain_format);
		void	   initialize_root_signature_and_pipeline();
		void	   initialize_vertex_and_index_buffers();
		void	   initialize_depth_image(Extent size);
		void	   update_cam(Tick tick);
		void	   on_mouse_move(MouseMoveEvent& event);
	};
}
