module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <memory>
#include <string>
#include <vector>
export module vt.Graphics.Vulkan.SyncTokenPool;

import vt.Core.Array;
import vt.Core.SmallList;
import vt.Graphics.Handle;
import vt.Graphics.Vulkan.Handle;
import vt.Trace.Log;

namespace vt::vulkan
{
	// Struct combining fences and semaphores into a unified type. The reset count acts as a versioning mechanism.
	struct UniqueSyncToken
	{
		UniqueVkFence	  fence;
		UniqueVkSemaphore semaphore;
		uint64_t		  resets = 0;

		UniqueSyncToken(DeviceApiTable const& api, VkFenceCreateFlags fence_flags)
		{
			VkFenceCreateInfo const fence_info {
				.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				.flags = fence_flags,
			};
			auto result = api.vkCreateFence(api.device, &fence_info, nullptr, std::out_ptr(fence, api));
			VT_CHECK_RESULT(result, "Failed to create Vulkan fence for queue.");

			VkSemaphoreCreateInfo const semaphore_info {
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			};
			result = api.vkCreateSemaphore(api.device, &semaphore_info, nullptr, std::out_ptr(semaphore, api));
			VT_CHECK_RESULT(result, "Failed to create Vulkan semaphore for queue.");
		}

		SyncToken get_unowned() const
		{
			return {
				fence.get(),
				semaphore.get(),
				resets,
			};
		}
	};

	export class SyncTokenPool
	{
	public:
		SyncTokenPool(DeviceApiTable const& api)
		{
			for(int i = 0; i != INITIAL_COUNT; ++i)
				tokens.emplace_back(api, VK_FENCE_CREATE_SIGNALED_BIT);
		}

		SyncToken acquire_token(DeviceApiTable const& api)
		{
			size_t size = tokens.size();
			size_t i	= current_index;
			do
			{
				auto& current_token = tokens[i];

				auto status = api.vkGetFenceStatus(api.device, current_token.fence.get());
				if(status == VK_SUCCESS)
				{
					auto fence	= current_token.fence.get();
					auto result = api.vkResetFences(api.device, 1, &fence);
					VT_CHECK_RESULT(result, "Failed to reset Vulkan fence.");

					++current_token.resets;
					advance_index();
					return current_token.get_unowned();
				}

				if(i == size - 1)
					i = 0;
				else
					++i;
			} while(i != current_index);

			// All existing tokens are somehow still in use, so grow the pool with an unsignaled token and return the new one.

			auto new_token = tokens.emplace(tokens.begin() + current_index, api, 0);
			Log().warn("The Vulkan sync token pool was grown. It is possible too much work is being submitted to the GPU.");

			advance_index();
			return new_token->get_unowned();
		}

		uint64_t get_current_resets(SyncToken const& token) const
		{
			auto it = std::find_if(tokens.begin(), tokens.end(), [&](UniqueSyncToken const& unique_token) {
				return unique_token.fence.get() == token.vulkan.fence;
			});
			return it->resets;
		}

		void await_all_fences(DeviceApiTable const& api) const
		{
			SmallList<VkFence> fences;
			fences.reserve(tokens.size());
			for(auto& token : tokens)
				fences.emplace_back(token.fence.get());

			auto result = api.vkWaitForFences(api.device, count(fences), fences.data(), true, UINT64_MAX);
			VT_CHECK_RESULT(result, "Failed to wait for Vulkan fences before sync token pool destruction.");
		}

		// Logs a visualization of the status of all fences in the pool. '_' means signaled, '!' means unsignaled.
		void log_fence_states(DeviceApiTable const& api) const
		{
			std::string states;
			states.reserve(tokens.size());
			for(auto& token : tokens)
			{
				auto state	= api.vkGetFenceStatus(api.device, token.fence.get());
				char symbol = state == VK_SUCCESS ? '_' : '!';
				states.push_back(symbol);
			}
			Log().info(states);
		}

	private:
		static constexpr size_t INITIAL_COUNT = 20;

		std::vector<UniqueSyncToken> tokens;
		size_t						 current_index = 0;

		void advance_index()
		{
			current_index = (current_index + 1) % tokens.size();
		}
	};
}
