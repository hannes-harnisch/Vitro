module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <cstdlib>
#include <ranges>
export module Vitro.D3D12.CommandList;

import Vitro.Core.Algorithm;
import Vitro.Core.FixedList;
import Vitro.Core.Rectangle;
import Vitro.D3D12.RenderPass;
import Vitro.D3D12.RenderTarget;
import Vitro.D3D12.Utils;
import Vitro.Graphics.CommandListBase;
import Vitro.Graphics.Device;
import Vitro.Graphics.PipelineInfo;
import Vitro.Graphics.RenderPass;
import Vitro.Graphics.RenderTarget;
import Vitro.Graphics.Resource;
import Vitro.Graphics.RootSignature;

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
		RenderPass const*	activeRenderPass   = nullptr;
		RenderTarget const* activeRenderTarget = nullptr;
		unsigned			subpassIndex	   = 1;
	};

	export template<CommandType Type> class CommandList final : public RenderCommandListBase, public CommandListData<Type>
	{
	public:
		CommandList(vt::Device const& device) : device(device.d3d12), allocator(makeAllocator()), cmd(makeCommandList())
		{}

		void* handle() override
		{
			return static_cast<ID3D12CommandList*>(cmd.get());
		}

		void reset() override
		{
			device.waitForFrameFence();

			auto result = allocator->Reset();
			vtAssertResult(result, "Failed to reset D3D12 command allocator.");
		}

		void begin() override
		{
			auto result = cmd->Reset(allocator.get(), nullptr);
			vtAssertResult(result, "Failed to reset D3D12 command list.");
		}

		void end() override
		{
			auto result = cmd->Close();
			vtAssertResult(result, "Failed to end D3D12 command list.");
		}

		void bindPipeline(vt::Pipeline const& pipeline) override
		{
			auto pipelineState = pipeline.d3d12.get();
			cmd->SetPipelineState(pipelineState);
		}

		void bindRootSignature(vt::RootSignature const& rootSignature) override
		{
			if constexpr(Type == CommandType::Render)
				cmd->SetGraphicsRootSignature(rootSignature.d3d12.get());
			else if constexpr(Type == CommandType::Compute)
				cmd->SetComputeRootSignature(rootSignature.d3d12.get());
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
			this->activeRenderPass	 = &renderPass.d3d12;
			this->activeRenderTarget = &renderTarget.d3d12;

			auto& pass	 = *this->activeRenderPass;
			auto& target = *this->activeRenderTarget;
			insertRenderPassBarriers(pass.subpasses[0]);

			FixedList<D3D12_RENDER_PASS_RENDER_TARGET_DESC, MaxColorAttachments> rtDescs;

			unsigned i = 0;
			for(auto attachment : std::span(pass.attachments.begin(), pass.attachments.end() - pass.usesDepthStencil))
			{
				rtDescs.emplace_back(D3D12_RENDER_PASS_RENDER_TARGET_DESC {
					.cpuDescriptor	 = target.getView(i++),
					.BeginningAccess = {.Type = attachment.beginAccess},
					.EndingAccess	 = {.Type = attachment.endAccess},
				});
			};

			D3D12_RENDER_PASS_DEPTH_STENCIL_DESC  dsDesc;
			D3D12_RENDER_PASS_DEPTH_STENCIL_DESC* dsDescPtr;
			if(pass.usesDepthStencil)
			{
				dsDesc = {
					.cpuDescriptor			= target.depthStencilView(),
					.DepthBeginningAccess	= {.Type = pass.attachments.back().beginAccess},
					.StencilBeginningAccess = {.Type = pass.stencilBeginAccess},
					.DepthEndingAccess		= {.Type = pass.attachments.back().endAccess},
					.StencilEndingAccess	= {.Type = pass.stencilEndAccess},
				};
				dsDescPtr = &dsDesc;
			}
			else
				dsDescPtr = nullptr;

			cmd->BeginRenderPass(count(rtDescs), rtDescs.data(), dsDescPtr, D3D12_RENDER_PASS_FLAG_NONE);
			this->subpassIndex = 1;
		}

		void transitionToNextSubpass() override
		{
			vtAssert(this->subpassIndex < this->activeRenderPass->subpasses.size() - 1,
					 "All subpasses of this render pass have already been transitioned through.");

			insertRenderPassBarriers(this->activeRenderPass->subpasses[this->subpassIndex++]);
		}

		void endRenderPass() override
		{
			cmd->EndRenderPass();
			insertRenderPassBarriers(this->activeRenderPass->finalTransitions);
#if VT_DEBUG
			this->activeRenderPass = nullptr;
#endif
		}

		void bindIndexBuffer(vt::Buffer const& buffer, IndexFormat format) override
		{
			D3D12_INDEX_BUFFER_VIEW const view {
				.BufferLocation = buffer.d3d12.get()->GetGPUVirtualAddress(),
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

		Device const&						  device;
		ComUnique<ID3D12CommandAllocator>	  allocator;
		ComUnique<ID3D12GraphicsCommandList4> cmd;

		ID3D12CommandAllocator* makeAllocator()
		{
			ID3D12CommandAllocator* alloc;

			auto result = device.get()->CreateCommandAllocator(CommandListType, IID_PPV_ARGS(&alloc));
			vtAssertResult(result, "Failed to create D3D12 command allocator.");

			return alloc;
		}

		ID3D12GraphicsCommandList4* makeCommandList()
		{
			ID3D12GraphicsCommandList4* list;

			auto result = device.get()->CreateCommandList(0, CommandListType, allocator.get(), nullptr, IID_PPV_ARGS(&list));
			vtAssertResult(result, "Failed to create D3D12 command list.");

			result = list->Close();
			vtAssertResult(result, "Failed to initially close D3D12 command list.");

			return list;
		}

		void insertRenderPassBarriers(FixedList<AttachmentTransition, MaxAttachments> const& transitions)
		{
			FixedList<D3D12_RESOURCE_BARRIER, MaxAttachments> barriers;
			for(auto transition : transitions)
			{
				barriers.emplace_back(D3D12_RESOURCE_BARRIER {
					.Transition {
						.pResource	 = this->activeRenderTarget->getAttachment(transition.index),
						.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
						.StateBefore = transition.oldLayout,
						.StateAfter	 = transition.newLayout,
					},
				});
			}
			cmd->ResourceBarrier(count(barriers), barriers.data());
		}
	};
}
