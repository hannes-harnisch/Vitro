module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <optional>
#include <ranges>
export module Vitro.D3D12.RenderPass;

import Vitro.Core.FixedList;
import Vitro.D3D12.Texture;
import Vitro.Graphics.Device;
import Vitro.Graphics.RenderPassInfo;

namespace vt::d3d12
{
	constexpr D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE convertImageLoadOp(ImageLoadOp op)
	{
		switch(op)
		{
			case ImageLoadOp::Load: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
			case ImageLoadOp::Clear: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
			case ImageLoadOp::Ignore: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
		}
		vtUnreachable();
	}

	constexpr D3D12_RENDER_PASS_ENDING_ACCESS_TYPE convertImageStoreOp(ImageStoreOp op)
	{
		switch(op)
		{
			case ImageStoreOp::Store: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
			case ImageStoreOp::Ignore: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
		}
		vtUnreachable();
	}

	struct AttachmentInfo
	{
		DXGI_FORMAT								format;
		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE beginAccess;
		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE	endAccess;

		AttachmentInfo(vt::AttachmentInfo info) :
			format(convertImageFormat(info.format)),
			beginAccess(convertImageLoadOp(info.loadOp)),
			endAccess(convertImageStoreOp(info.storeOp))
		{}
	};

	export struct AttachmentTransition
	{
		unsigned			  index;
		D3D12_RESOURCE_STATES oldLayout;
		D3D12_RESOURCE_STATES newLayout;
	};

	export using TransitionList = FixedList<AttachmentTransition, MaxAttachments>;

	export class RenderPass
	{
	public:
		FixedList<AttachmentInfo, MaxAttachments> attachments;
		bool									  usesDepthStencil;
		D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE	  stencilBeginAccess;
		D3D12_RENDER_PASS_ENDING_ACCESS_TYPE	  stencilEndAccess;
		FixedList<TransitionList, MaxSubpasses>	  subpasses;
		TransitionList							  finalTransitions;

		RenderPass(vt::Device const&, RenderPassInfo const& info) :
			attachments(info.attachments.begin(), info.attachments.end()),
			usesDepthStencil(containsDepthStencilAttachment(info.attachments)),
			stencilBeginAccess(convertImageLoadOp(info.stencilLoadOp)),
			stencilEndAccess(convertImageStoreOp(info.stencilStoreOp))
		{
			FixedList<D3D12_RESOURCE_STATES, MaxAttachments> prevLayouts;
			for(auto attachment : info.attachments)
				prevLayouts.emplace_back(convertImageLayout(attachment.initialLayout));

			for(auto& subpass : info.subpasses)
			{
				TransitionList transitions;
				for(auto ref : subpass.outputRefs)
				{
					auto oldLayout = prevLayouts[ref.index];
					auto newLayout = convertImageLayout(ref.usedLayout);
					if(oldLayout == newLayout)
						continue;

					transitions.emplace_back(ref.index, oldLayout, newLayout);
					prevLayouts[ref.index] = newLayout;
				}
				subpasses.emplace_back(transitions);
			}

			for(unsigned i = 0; i != info.attachments.size(); ++i)
			{
				auto prev  = prevLayouts[i];
				auto final = convertImageLayout(info.attachments[i].finalLayout);
				if(prev == final)
					continue;

				finalTransitions.emplace_back(i, prev, final);
			}
		}

	private:
		static bool containsDepthStencilAttachment(auto& attachments)
		{
			auto usesDepthStencilLayout = [](auto attachment) {
				return attachment.finalLayout == ImageLayout::DepthStencilAttachment ||
					   attachment.finalLayout == ImageLayout::DepthStencilReadOnly;
			};
			auto it = std::find_if(attachments.begin(), attachments.end(), usesDepthStencilLayout);

			auto second = std::find_if(it, attachments.end(), usesDepthStencilLayout);
			vtAssert(second == attachments.end(), "Multiple depth stencil attachments are unsupported.");

			return it != attachments.end();
		}
	};
}
