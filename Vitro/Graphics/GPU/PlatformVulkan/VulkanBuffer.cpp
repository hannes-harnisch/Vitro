module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <memory>
export module vt.Graphics.Vulkan.Buffer;

import vt.Core.Array;
import vt.Graphics.AssetResourceSpecification;
import vt.Graphics.Vulkan.Handle;

namespace vt::vulkan
{
	VkBufferUsageFlags convert_buffer_usage(BufferUsage usage)
	{
		VkBufferUsageFlags flags = 0;

		using enum BufferUsage;
		if(usage & CopySrc)
			flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		if(usage & CopyDst)
			flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		if(usage & Untyped)
			flags |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
		if(usage & RwUntyped)
			flags |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
		if(usage & Uniform)
			flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		if(usage & Storage)
			flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		if(usage & Index)
			flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		if(usage & Vertex)
			flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		if(usage & Indirect)
			flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

		return flags;
	}

	export class VulkanBuffer
	{
	public:
		VulkanBuffer(BufferSpecification const& spec, DeviceApiTable const& api, VmaAllocator allocator)
		{
			VmaMemoryUsage usage;
			if(spec.usage.get() & BufferUsage::Upload)
				usage = VMA_MEMORY_USAGE_CPU_ONLY;
			else if(spec.usage.get() & BufferUsage::Readback)
				usage = VMA_MEMORY_USAGE_GPU_TO_CPU;
			else
				usage = VMA_MEMORY_USAGE_GPU_ONLY;

			VmaAllocationCreateInfo const allocation_info {
				.flags			= 0,
				.usage			= usage,
				.requiredFlags	= 0,
				.preferredFlags = 0,
				.memoryTypeBits = 0,
				.pool			= nullptr,
				.pUserData		= nullptr,
				.priority		= 0,
			};

			VkBufferCreateInfo const buffer_info {
				.sType				   = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
				.size				   = spec.size,
				.usage				   = convert_buffer_usage(spec.usage),
				.sharingMode		   = VK_SHARING_MODE_EXCLUSIVE,
				.queueFamilyIndexCount = 0,
				.pQueueFamilyIndices   = nullptr,
			};
			auto result = vmaCreateBuffer(allocator, &buffer_info, &allocation_info, std::out_ptr(buffer),
										  &buffer.get_deleter().allocation, nullptr);

			buffer.get_deleter().api = &api;
			VT_CHECK_RESULT(result, "Failed to create Vulkan buffer.");
		}

		VkBuffer get_handle() const
		{
			return buffer.get();
		}

		VmaAllocation get_allocation() const
		{
			return buffer.get_deleter().allocation;
		}

	private:
		struct BufferDeleter
		{
			using pointer = VkBuffer;

			DeviceApiTable const* api;
			VmaAllocation		  allocation;

			void operator()(VkBuffer buffer) const
			{
				vmaDestroyBuffer(api->allocator, buffer, allocation);
			}
		};
		using UniqueVkBuffer = std::unique_ptr<VkBuffer, BufferDeleter>;

		UniqueVkBuffer buffer;
	};
}
