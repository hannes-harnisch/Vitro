module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"
export module Vitro.D3D12.CommandList;

import Vitro.D3D12.ComUnique;
import Vitro.D3D12.RenderPass;
import Vitro.Graphics.CommandListBase;
import Vitro.Graphics.Device;
import Vitro.Graphics.Handle;
import Vitro.Math.Rectangle;

namespace vt::d3d12
{
	consteval D3D12_COMMAND_LIST_TYPE mapQueuePurposeToCommandListType(QueuePurpose purpose)
	{
		switch(purpose)
		{
			case QueuePurpose::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
			case QueuePurpose::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
			case QueuePurpose::Render: return D3D12_COMMAND_LIST_TYPE_DIRECT;
		}
	}

	template<QueuePurpose Purpose> class CommandListData
	{};

	template<> class CommandListData<QueuePurpose::Compute> : public CommandListData<QueuePurpose::Copy>
	{};

	template<> class CommandListData<QueuePurpose::Render> : public CommandListData<QueuePurpose::Compute>
	{
	protected:
		RenderPass* currentRenderPass		= nullptr;
		ID3D12Resource* currentRenderTarget = nullptr;
		unsigned subpassIndex				= 1;
	};

	export template<QueuePurpose Purpose>
	class CommandList final : public RenderCommandListBase, public CommandListData<Purpose>
	{
	public:
		CommandList(vt::Device& device) : allocator(makeAllocator(device)), cmd(makeCommandList(device))
		{}

		void begin() override
		{
			auto result = allocator->Reset();
			vtAssertResult(result, "Failed to reset D3D12 command allocator.");
		}

		void end() override
		{
			auto result = cmd->Close();
			vtAssertResult(result, "Failed to end D3D12 command list.");
		}

		void bindPipeline(vt::PipelineHandle const pipeline) override
		{
			cmd->SetPipelineState(pipeline.d3d12.handle);
		}

		void bindRootSignature(vt::RootSignatureHandle const rootSignature) override
		{
			if constexpr(Purpose == QueuePurpose::Render)
				cmd->SetGraphicsRootSignature(rootSignature.d3d12.handle);
			else if constexpr(Purpose == QueuePurpose::Compute)
				cmd->SetComputeRootSignature(rootSignature.d3d12.handle);
			else
				static_assert(false, "This command is not supported on copy command lists.");
		}

		void dispatch(unsigned const xCount, unsigned const yCount, unsigned const zCount) override
		{
			cmd->Dispatch(xCount, yCount, zCount);
		}

		void beginRenderPass(vt::RenderPassHandle const renderPass, vt::RenderTargetHandle const renderTarget) override
		{
			cmd->OMSetRenderTargets(1, &renderTarget.d3d12.rtv, true, &renderTarget.d3d12.dsv);
			this->currentRenderPass	  = renderPass.d3d12.handle;
			this->currentRenderTarget = renderTarget.d3d12.resource;

			auto const pass = this->currentRenderPass;
			D3D12_RENDER_PASS_RENDER_TARGET_DESC const rtDesc {
				.cpuDescriptor	 = renderTarget.d3d12.rtv,
				.BeginningAccess = pass->colorBeginAccess,
				.EndingAccess	 = pass->colorEndAccess,
			};
			D3D12_RENDER_PASS_DEPTH_STENCIL_DESC const dsDesc {
				.cpuDescriptor			= renderTarget.d3d12.dsv,
				.DepthBeginningAccess	= pass->depthBeginAccess,
				.StencilBeginningAccess = pass->stencilBeginAccess,
				.DepthEndingAccess		= pass->depthEndAccess,
				.StencilEndingAccess	= pass->stencilEndAccess,
			};
			cmd->BeginRenderPass(1, &rtDesc, &dsDesc, pass->flags);

			auto const& transition = pass->transitions.front();
			D3D12_RESOURCE_BARRIER const barrier {
				.Transition =
					{
						.pResource	 = this->currentRenderTarget,
						.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
						.StateBefore = transition.before,
						.StateAfter	 = transition.after,
					},
			};
			cmd->ResourceBarrier(1, &barrier);
		}

		void transitionToNextSubpass() override
		{
			vtAssert(this->subpassIndex < this->currentRenderPass->transitions.size() - 1,
					 "All subpasses of this render pass have already been transitioned through.");

			auto const& transition = this->currentRenderPass->transitions[this->subpassIndex++];
			D3D12_RESOURCE_BARRIER const barrier {
				.Transition =
					{
						.pResource	 = this->currentRenderTarget,
						.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
						.StateBefore = transition.before,
						.StateAfter	 = transition.after,
					},
			};
			cmd->ResourceBarrier(1, &barrier);
		}

		void endRenderPass() override
		{
			auto const& transition = this->currentRenderPass->transitions.back();
			D3D12_RESOURCE_BARRIER const barrier {
				.Transition =
					{
						.pResource	 = this->currentRenderTarget,
						.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
						.StateBefore = transition.before,
						.StateAfter	 = transition.after,
					},
			};
			cmd->ResourceBarrier(1, &barrier);

			cmd->EndRenderPass();
			this->subpassIndex = 1;
		}

		void bindViewport(Viewport const viewport) override
		{
			cmd->RSSetViewports(1, reinterpret_cast<D3D12_VIEWPORT const*>(&viewport));
		}

		void bindScissor(Rectangle const scissor) override
		{
			D3D12_RECT const rect {
				.left	= scissor.x,
				.top	= scissor.y,
				.right	= static_cast<LONG>(scissor.x + scissor.width),
				.bottom = static_cast<LONG>(scissor.y + scissor.height),
			};
			cmd->RSSetScissorRects(1, &rect);
		}

		void draw(unsigned const vertexCount,
				  unsigned const instanceCount,
				  unsigned const firstVertex,
				  unsigned const firstInstance) override
		{
			cmd->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance);
		}

		void drawIndexed(unsigned const indexCount,
						 unsigned const instanceCount,
						 unsigned const firstIndex,
						 int const vertexOffset,
						 unsigned const firstInstance) override
		{
			cmd->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
		}

	private:
		constexpr static D3D12_COMMAND_LIST_TYPE Type = mapQueuePurposeToCommandListType(Purpose);

		ComUnique<ID3D12CommandAllocator> allocator;
		ComUnique<ID3D12GraphicsCommandList4> cmd;

		static ComUnique<ID3D12CommandAllocator> makeAllocator(vt::Device& device)
		{
			ComUnique<ID3D12CommandAllocator> allocator;
			auto result = device.d3d12.handle()->CreateCommandAllocator(Type, IID_PPV_ARGS(&allocator));
			vtAssertResult(result, "Failed to create D3D12 command allocator.");
			return allocator;
		}

		ComUnique<ID3D12GraphicsCommandList4> makeCommandList(vt::Device& device)
		{
			ComUnique<ID3D12GraphicsCommandList4> list;
			auto result = device.d3d12.handle()->CreateCommandList(0, Type, allocator, nullptr, IID_PPV_ARGS(&list));
			vtAssertResult(result, "Failed to create D3D12 command list.");
			return list;
		}
	};
}
