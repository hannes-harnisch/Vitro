module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"
export module Vitro.D3D12.CommandList;

import Vitro.Core.Rectangle;
import Vitro.D3D12.RenderPass;
import Vitro.D3D12.Utils;
import Vitro.Graphics.CommandListBase;
import Vitro.Graphics.Device;
import Vitro.Graphics.Handle;
import Vitro.Graphics.PipelineInfo;
import Vitro.Graphics.RenderPass;
import Vitro.Graphics.RenderPassBase;
import Vitro.Graphics.RenderTarget;

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

	constexpr D3D_PRIMITIVE_TOPOLOGY convertPrimitiveTopology(PrimitiveTopology topology)
	{
		using enum PrimitiveTopology;
		switch(topology)
		{
			case PointList: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
			case LineList: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			case LineStrip: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
			case LineListWithAdjacency: return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
			case LineStripWithAdjacency: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
			case TriangleList: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			case TriangleStrip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			case TriangleListWithAdjacency: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
			case TriangleStripWithAdjacency: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
			case PatchList_1ControlPoint: return D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
			case PatchList_2ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
			case PatchList_3ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
			case PatchList_4ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
			case PatchList_5ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST;
			case PatchList_6ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST;
			case PatchList_7ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST;
			case PatchList_8ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST;
			case PatchList_9ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST;
			case PatchList_10ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST;
			case PatchList_11ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST;
			case PatchList_12ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST;
			case PatchList_13ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST;
			case PatchList_14ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST;
			case PatchList_15ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST;
			case PatchList_16ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST;
			case PatchList_17ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST;
			case PatchList_18ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST;
			case PatchList_19ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST;
			case PatchList_20ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST;
			case PatchList_21ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST;
			case PatchList_22ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST;
			case PatchList_23ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST;
			case PatchList_24ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST;
			case PatchList_25ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;
			case PatchList_26ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST;
			case PatchList_27ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST;
			case PatchList_28ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST;
			case PatchList_29ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST;
			case PatchList_30ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST;
			case PatchList_31ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST;
			case PatchList_32ControlPoints: return D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST;
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
		ID3D12Resource*	  activeColorAttachments[RenderPassBase::MaxColorAttachments] {};
		ID3D12Resource*	  activeDepthStencilAttachment = nullptr;
		RenderPass const* activeRenderPass			   = nullptr;
		unsigned		  subpassIndex				   = 1;
	};

	export template<CommandType Type> class CommandList final : public RenderCommandListBase, public CommandListData<Type>
	{
	public:
		CommandList(vt::Device& device) :
			allocator(makeAllocator(device->handle().d3d12())), cmd(makeCommandList(device->handle().d3d12()))
		{}

		CommandListHandle handle() override
		{
			return {cmd.get()};
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
			auto pipelineState = pipeline.d3d12();
			cmd->SetPipelineState(pipelineState);
		}

		void bindRootSignature(RootSignatureHandle rootSignature) override
		{
			if constexpr(Type == CommandType::Render)
				cmd->SetGraphicsRootSignature(rootSignature.d3d12());
			else if constexpr(Type == CommandType::Compute)
				cmd->SetComputeRootSignature(rootSignature.d3d12());
			else
				static_assert(false, "This command is not supported on copy command lists.");
		}

		void pushConstants(void const* data, unsigned size, unsigned offset) override
		{
			if constexpr(Type == CommandType::Render)
				cmd->SetGraphicsRoot32BitConstants(0, size / sizeof(DWORD), data, offset / sizeof(DWORD));
			else if constexpr(Type == CommandType::Compute)
				cmd->SetComputeRoot32BitConstants(0, size / sizeof(DWORD), data, offset / sizeof(DWORD));
			else
				static_assert(false, "This command is not supported on copy command lists.");
		}

		void dispatch(unsigned xCount, unsigned yCount, unsigned zCount) override
		{
			cmd->Dispatch(xCount, yCount, zCount);
		}

		void beginRenderPass(vt::RenderPass const& renderPass, vt::RenderTarget const& renderTarget) override
		{
			auto pass = this->activeRenderPass = &renderPass.d3d12;
			this->currentRenderTarget		   = renderTarget.d3d12.resource();

			auto const& transition = pass->transitions.front();

			D3D12_RESOURCE_BARRIER const barrier {
				.Transition {
					.pResource	 = this->currentRenderTarget,
					.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
					.StateBefore = transition.before,
					.StateAfter	 = transition.after,
				},
			};
			cmd->ResourceBarrier(1, &barrier);

			D3D12_RENDER_PASS_RENDER_TARGET_DESC const rtDesc {
				.cpuDescriptor	 = renderTarget.d3d12.rtv(),
				.BeginningAccess = pass->colorBeginAccess,
				.EndingAccess	 = pass->colorEndAccess,
			};
			D3D12_RENDER_PASS_DEPTH_STENCIL_DESC const dsDesc {
				.cpuDescriptor			= renderTarget.d3d12.dsv(),
				.DepthBeginningAccess	= pass->depthBeginAccess,
				.StencilBeginningAccess = pass->stencilBeginAccess,
				.DepthEndingAccess		= pass->depthEndAccess,
				.StencilEndingAccess	= pass->stencilEndAccess,
			};
			cmd->BeginRenderPass(1, &rtDesc, &dsDesc, 0);
		}

		void transitionToNextSubpass() override
		{
			vtAssert(this->subpassIndex < this->activeRenderPass->subpassCount - 1,
					 "All subpasses of this render pass have already been transitioned through.");

			auto const& transition = this->activeRenderPass->transitions[this->subpassIndex++];

			D3D12_RESOURCE_BARRIER const barrier {
				.Transition {
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
#if VT_DEBUG
			this->activeRenderPass = nullptr;
#endif

			auto const& transition = this->activeRenderPass->transitions.back();

			D3D12_RESOURCE_BARRIER const barrier {
				.Transition {
					.pResource	 = this->currentRenderTarget,
					.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
					.StateBefore = transition.before,
					.StateAfter	 = transition.after,
				},
			};
			cmd->ResourceBarrier(1, &barrier);
		}

		void bindIndexBuffer(BufferHandle buffer, IndexFormat format) override
		{
			D3D12_INDEX_BUFFER_VIEW const view {
				.BufferLocation = buffer.d3d12()->GetGPUVirtualAddress(),
				.SizeInBytes	= 0, // TODO?
				.Format			= convertIndexFormat(format),
			};
			cmd->IASetIndexBuffer(&view);
		}

		void bindPrimitiveTopology(PrimitiveTopology topology) override
		{
			cmd->IASetPrimitiveTopology(convertPrimitiveTopology(topology));
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
						 int	  vertexOffset,
						 unsigned firstInstance) override
		{
			cmd->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
		}

	private:
		static constexpr D3D12_COMMAND_LIST_TYPE CommandListType = convertCommandType(Type);

		UniqueInterface<ID3D12CommandAllocator>		allocator;
		UniqueInterface<ID3D12GraphicsCommandList4> cmd;

		static ID3D12CommandAllocator* makeAllocator(ID3D12Device1* device)
		{
			ID3D12CommandAllocator* allocator;

			auto result = device->CreateCommandAllocator(CommandListType, IID_PPV_ARGS(&allocator));
			vtAssertResult(result, "Failed to create D3D12 command allocator.");

			return allocator;
		}

		ID3D12GraphicsCommandList4* makeCommandList(ID3D12Device1* device)
		{
			ID3D12GraphicsCommandList4* list;

			auto result = device->CreateCommandList(0, CommandListType, allocator.get(), nullptr, IID_PPV_ARGS(&list));
			vtAssertResult(result, "Failed to create D3D12 command list.");

			return list;
		}
	};
}
