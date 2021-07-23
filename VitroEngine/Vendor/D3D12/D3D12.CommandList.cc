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
	constexpr D3D12_COMMAND_LIST_TYPE convertCommandType(CommandType type)
	{
		switch(type)
		{
			case CommandType::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
			case CommandType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
			case CommandType::Render: return D3D12_COMMAND_LIST_TYPE_DIRECT;
		}
		vtUnreachable();
	}

	constexpr DXGI_FORMAT convertIndexFormat(IndexFormat format)
	{
		switch(format)
		{
			case IndexFormat::UInt16: return DXGI_FORMAT_R16_UINT;
			case IndexFormat::UInt32: return DXGI_FORMAT_R32_UINT;
		}
		vtUnreachable();
	}

	template<CommandType Type> class CommandListData
	{};

	template<> class CommandListData<CommandType::Compute> : public CommandListData<CommandType::Copy>
	{};

	template<> class CommandListData<CommandType::Render> : public CommandListData<CommandType::Compute>
	{
	protected:
		RenderPass* currentRenderPass		= nullptr;
		ID3D12Resource* currentRenderTarget = nullptr;
		unsigned subpassIndex				= 1;
	};

	export template<CommandType Type> class CommandList final : public RenderCommandListBase, public CommandListData<Type>
	{
	public:
		CommandList(vt::Device& device) :
			allocator(makeAllocator(device.d3d12.handle())), cmd(makeCommandList(device.d3d12.handle()))
		{}

		CommandListHandle handle() override
		{
			return {cmd};
		}

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

		void bindPipeline(PipelineHandle pipeline) override
		{
			cmd->SetPipelineState(pipeline.d3d12.handle);

			if constexpr(Type == CommandType::Render)
				cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}

		void bindRootSignature(RootSignatureHandle rootSignature) override
		{
			if constexpr(Type == CommandType::Render)
				cmd->SetGraphicsRootSignature(rootSignature.d3d12.handle);
			else if constexpr(Type == CommandType::Compute)
				cmd->SetComputeRootSignature(rootSignature.d3d12.handle);
			else
				static_assert(false, "This command is not supported on copy command lists.");
		}

		void pushConstants(void const* data, unsigned size, unsigned offset) override
		{
			if constexpr(Type == CommandType::Render)
				cmd->SetGraphicsRoot32BitConstants(0, size / sizeof(int), data, offset / sizeof(int));
			else if constexpr(Type == CommandType::Compute)
				cmd->SetComputeRoot32BitConstants(0, size / sizeof(int), data, offset / sizeof(int));
			else
				static_assert(false, "This command is not supported on copy command lists.");
		}

		void dispatch(unsigned xCount, unsigned yCount, unsigned zCount) override
		{
			cmd->Dispatch(xCount, yCount, zCount);
		}

		void beginRenderPass(RenderPassHandle renderPass, RenderTargetHandle renderTarget) override
		{
			this->currentRenderPass	  = renderPass.d3d12.handle;
			this->currentRenderTarget = renderTarget.d3d12.resource;

			auto const& transition = this->currentRenderPass->transitions.front();
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

			D3D12_RENDER_PASS_RENDER_TARGET_DESC const rtDesc {
				.cpuDescriptor	 = renderTarget.d3d12.rtv,
				.BeginningAccess = this->currentRenderPass->colorBeginAccess,
				.EndingAccess	 = this->currentRenderPass->colorEndAccess,
			};
			D3D12_RENDER_PASS_DEPTH_STENCIL_DESC const dsDesc {
				.cpuDescriptor			= renderTarget.d3d12.dsv,
				.DepthBeginningAccess	= this->currentRenderPass->depthBeginAccess,
				.StencilBeginningAccess = this->currentRenderPass->stencilBeginAccess,
				.DepthEndingAccess		= this->currentRenderPass->depthEndAccess,
				.StencilEndingAccess	= this->currentRenderPass->stencilEndAccess,
			};
			cmd->BeginRenderPass(1, &rtDesc, &dsDesc, this->currentRenderPass->flags);
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
			cmd->EndRenderPass();
			this->subpassIndex = 1;

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
		}

		void bindIndexBuffer(BufferHandle buffer, IndexFormat format, unsigned offset) override
		{
			D3D12_INDEX_BUFFER_VIEW const view {
				.BufferLocation = buffer.d3d12.handle->GetGPUVirtualAddress() + offset,
				.SizeInBytes	= 0, // TODO?
				.Format			= convertIndexFormat(format),
			};
			cmd->IASetIndexBuffer(&view);
		}

		void bindViewport(Viewport const viewport) override
		{
			auto data = reinterpret_cast<D3D12_VIEWPORT const*>(&viewport);
			cmd->RSSetViewports(1, data);
		}

		void bindScissor(Rectangle scissor) override
		{
			D3D12_RECT const rect {
				.left	= scissor.x,
				.top	= scissor.y,
				.right	= static_cast<LONG>(scissor.x + scissor.width),
				.bottom = static_cast<LONG>(scissor.y + scissor.height),
			};
			cmd->RSSetScissorRects(1, &rect);
		}

		void draw(unsigned vertexCount, unsigned instanceCount, unsigned firstVertex, unsigned firstInstance) override
		{
			cmd->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance);
		}

		void drawIndexed(unsigned indexCount,
						 unsigned instanceCount,
						 unsigned firstIndex,
						 int vertexOffset,
						 unsigned firstInstance) override
		{
			cmd->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
		}

	private:
		static constexpr D3D12_COMMAND_LIST_TYPE D3D12CommandType = convertCommandType(Type);

		ComUnique<ID3D12CommandAllocator> allocator;
		ComUnique<ID3D12GraphicsCommandList4> cmd;

		static ComUnique<ID3D12CommandAllocator> makeAllocator(ID3D12Device1* device)
		{
			ComUnique<ID3D12CommandAllocator> allocator;
			auto result = device->CreateCommandAllocator(D3D12CommandType, IID_PPV_ARGS(&allocator));
			vtAssertResult(result, "Failed to create D3D12 command allocator.");
			return allocator;
		}

		ComUnique<ID3D12GraphicsCommandList4> makeCommandList(ID3D12Device1* device)
		{
			ComUnique<ID3D12GraphicsCommandList4> list;
			auto result = device->CreateCommandList(0, D3D12CommandType, allocator, nullptr, IID_PPV_ARGS(&list));
			vtAssertResult(result, "Failed to create D3D12 command list.");
			return list;
		}
	};
}
