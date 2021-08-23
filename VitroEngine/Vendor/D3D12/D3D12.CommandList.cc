module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <cstdlib>
#include <ranges>
#include <type_traits>
export module Vitro.D3D12.CommandList;

import Vitro.Core.Algorithm;
import Vitro.Core.FixedList;
import Vitro.Core.Rectangle;
import Vitro.D3D12.RenderPass;
import Vitro.D3D12.RenderTarget;
import Vitro.D3D12.Utils;
import Vitro.Graphics.CommandListBase;
import Vitro.Graphics.Device;
import Vitro.Graphics.Driver;
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

	constexpr DXGI_FORMAT getIndexFormatFromStride(unsigned stride)
	{
		switch(stride)
		{
			case 2: return DXGI_FORMAT_R16_UINT;
			case 4: return DXGI_FORMAT_R32_UINT;
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

	template<CommandType Type> class CommandListData;

	template<> class CommandListData<CommandType::Copy>
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

	template<CommandType Type>
	using CommandListBase = std::conditional_t<
		Type == CommandType::Render,
		RenderCommandListBase,
		std::conditional_t<Type == CommandType::Compute,
						   ComputeCommandListBase,
						   std::conditional_t<Type == CommandType::Copy, CopyCommandListBase, void>>>;

	export template<CommandType Type> class CommandList final : public CommandListBase<Type>, public CommandListData<Type>
	{
	public:
		CommandList(vt::Device const& device) :
			allocator(makeAllocator(device.d3d12.get())), cmd(makeCommandList(device.d3d12.get()))
		{}

		vt::CommandListHandle handle()
		{
			return {cmd.get()};
		}

		void reset()
		{
			auto result = allocator->Reset();
			vtAssertResult(result, "Failed to reset D3D12 command allocator.");
		}

		void begin()
		{
			auto result = cmd->Reset(allocator.get(), nullptr);
			vtAssertResult(result, "Failed to reset D3D12 command list.");
		}

		void end()
		{
			auto result = cmd->Close();
			vtAssertResult(result, "Failed to end D3D12 command list.");
		}

		void bindComputePipeline(vt::Pipeline const& pipeline)
		{
			cmd->SetPipelineState(pipeline.d3d12.get());
		}

		void bindComputeRootSignature(vt::RootSignature const& rootSignature)
		{
			cmd->SetComputeRootSignature(rootSignature.d3d12.get());
		}

		void pushComputeConstants(void const* data, unsigned size, unsigned offset)
		{
			cmd->SetComputeRoot32BitConstants(0, size / sizeof(DWORD), data, offset / sizeof(DWORD));
		}

		void dispatch(unsigned xCount, unsigned yCount, unsigned zCount)
		{
			cmd->Dispatch(xCount, yCount, zCount);
		}

		void bindRenderPipeline(vt::Pipeline const& pipeline)
		{
			cmd->SetPipelineState(pipeline.d3d12.get());
		}

		void bindRenderRootSignature(vt::RootSignature const& rootSignature)
		{
			cmd->SetGraphicsRootSignature(rootSignature.d3d12.get());
		}

		void pushRenderConstants(void const* data, unsigned size, unsigned offset)
		{
			cmd->SetGraphicsRoot32BitConstants(0, size / sizeof(DWORD), data, offset / sizeof(DWORD));
		}

		void beginRenderPass(vt::RenderPass const&		 renderPass,
							 vt::RenderTarget const&	 renderTarget,
							 std::span<ClearValue const> clearValues = {})
		{
			this->activeRenderPass	 = &renderPass.d3d12;
			this->activeRenderTarget = &renderTarget.d3d12;

			auto& pass	 = *this->activeRenderPass;
			auto& target = *this->activeRenderTarget;
			insertRenderPassBarriers(pass.subpasses[0]);

			FixedList<D3D12_RENDER_PASS_RENDER_TARGET_DESC, MaxColorAttachments> rtDescs;

			unsigned index = 0;
			for(auto attachment : std::span(pass.attachments.begin(), pass.attachments.end() - pass.usesDepthStencil))
			{
				D3D12_CLEAR_VALUE colorClear;
				if(clearValues.empty())
					colorClear = {};
				else
					colorClear = {
						.Format = attachment.format,
						.Color {
							clearValues[index].color.r,
							clearValues[index].color.g,
							clearValues[index].color.b,
							clearValues[index].color.a,
						},
					};

				rtDescs.emplace_back(D3D12_RENDER_PASS_RENDER_TARGET_DESC {
					.cpuDescriptor = target.getView(index),
					.BeginningAccess {
						.Type  = attachment.beginAccess,
						.Clear = {colorClear},
					},
					.EndingAccess = {.Type = attachment.endAccess},
				});
				++index;
			}

			D3D12_RENDER_PASS_DEPTH_STENCIL_DESC dsDesc;
			if(pass.usesDepthStencil)
			{
				D3D12_CLEAR_VALUE depthClear, stencilClear;
				if(clearValues.empty())
					depthClear = stencilClear = {};
				else
				{
					depthClear = {
						.Format		  = pass.attachments.back().format,
						.DepthStencil = {.Depth = clearValues.back().depth},
					};
					stencilClear = {
						.Format		  = pass.attachments.back().format,
						.DepthStencil = {.Stencil = clearValues.back().stencil},
					};
				}

				dsDesc = {
					.cpuDescriptor = target.depthStencilView(),
					.DepthBeginningAccess {
						.Type  = pass.attachments.back().beginAccess,
						.Clear = {depthClear},
					},
					.StencilBeginningAccess {
						.Type  = pass.stencilBeginAccess,
						.Clear = {stencilClear},
					},
					.DepthEndingAccess	 = {.Type = pass.attachments.back().endAccess},
					.StencilEndingAccess = {.Type = pass.stencilEndAccess},
				};
			}

			auto dsDescPtr = pass.usesDepthStencil ? &dsDesc : nullptr;
			cmd->BeginRenderPass(count(rtDescs), rtDescs.data(), dsDescPtr, D3D12_RENDER_PASS_FLAG_NONE);
			this->subpassIndex = 1;
		}

		void transitionToNextSubpass()
		{
			vtAssert(this->subpassIndex < this->activeRenderPass->subpasses.size() - 1,
					 "All subpasses of this render pass have already been transitioned through.");

			insertRenderPassBarriers(this->activeRenderPass->subpasses[this->subpassIndex++]);
		}

		void endRenderPass()
		{
			cmd->EndRenderPass();
			insertRenderPassBarriers(this->activeRenderPass->finalTransitions);
#if VT_DEBUG
			this->activeRenderPass = nullptr;
#endif
		}

		void bindVertexBuffers(unsigned firstBuffer, std::span<vt::Buffer const> buffers, std::span<unsigned const> byteOffsets)
		{
			FixedList<D3D12_VERTEX_BUFFER_VIEW, MaxVertexAttributes> views(firstBuffer);

			auto offset = byteOffsets.begin() + firstBuffer;
			for(auto& buffer : std::views::take(buffers, firstBuffer))
				views.emplace_back(D3D12_VERTEX_BUFFER_VIEW {
					.BufferLocation = buffer.d3d12.getGpuAddress() + *offset++,
					.SizeInBytes	= buffer.d3d12.getSize(),
					.StrideInBytes	= buffer.d3d12.getStride(),
				});

			cmd->IASetVertexBuffers(firstBuffer, count(views), views.data());
		}

		void bindIndexBuffer(vt::Buffer const& buffer, unsigned byteOffset)
		{
			D3D12_INDEX_BUFFER_VIEW const view {
				.BufferLocation = buffer.d3d12.getGpuAddress() + byteOffset,
				.SizeInBytes	= buffer.d3d12.getSize(),
				.Format			= getIndexFormatFromStride(buffer.d3d12.getStride()),
			};
			cmd->IASetIndexBuffer(&view);
		}

		void bindPrimitiveTopology(PrimitiveTopology topology)
		{
			cmd->IASetPrimitiveTopology(convertPrimitiveTopology(topology));
		}

		void bindViewports(std::span<Viewport const> viewports)
		{
			auto data = reinterpret_cast<D3D12_VIEWPORT const*>(viewports.data());
			cmd->RSSetViewports(count(viewports), data);
		}

		void bindScissors(std::span<Rectangle const> scissors)
		{
			FixedList<D3D12_RECT, MaxAttachments> rects;
			for(auto&& scissor : scissors)
				rects.emplace_back(D3D12_RECT {
					.left	= scissor.x,
					.top	= scissor.y,
					.right	= static_cast<LONG>(scissor.x + scissor.width),
					.bottom = static_cast<LONG>(scissor.y + scissor.height),
				});

			cmd->RSSetScissorRects(count(rects), rects.data());
		}

		void draw(unsigned vertexCount, unsigned instanceCount, unsigned firstVertex, unsigned firstInstance)
		{
			cmd->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance);
		}

		void drawIndexed(unsigned indexCount,
						 unsigned instanceCount,
						 unsigned firstIndex,
						 int	  vertexOffset,
						 unsigned firstInstance)
		{
			cmd->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
		}

	private:
		static constexpr D3D12_COMMAND_LIST_TYPE CommandListType = convertCommandType(Type);

		ComUnique<ID3D12CommandAllocator>	  allocator;
		ComUnique<ID3D12GraphicsCommandList4> cmd;

		static decltype(allocator) makeAllocator(ID3D12Device4* device)
		{
			ID3D12CommandAllocator* rawAllocator;

			auto result = device->CreateCommandAllocator(CommandListType, IID_PPV_ARGS(&rawAllocator));

			decltype(allocator) freshAllocator(rawAllocator);
			vtAssertResult(result, "Failed to create D3D12 command allocator.");

			return freshAllocator;
		}

		static decltype(cmd) makeCommandList(ID3D12Device4* device)
		{
			ID3D12GraphicsCommandList4* rawCmd;
			auto result = device->CreateCommandList1(0, CommandListType, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&rawCmd));
			decltype(cmd) freshCmd(rawCmd);

			vtAssertResult(result, "Failed to create D3D12 command list.");
			return freshCmd;
		}

		void insertRenderPassBarriers(FixedList<AttachmentTransition, MaxAttachments> const& transitions)
		{
			FixedList<D3D12_RESOURCE_BARRIER, MaxAttachments> barriers;
			for(auto transition : transitions)
			{
				barriers.emplace_back(D3D12_RESOURCE_BARRIER {
					.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
					.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
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
