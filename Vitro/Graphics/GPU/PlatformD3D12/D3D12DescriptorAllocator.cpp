module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <algorithm>
#include <deque>
export module vt.Graphics.D3D12.DescriptorAllocator;

import vt.Graphics.D3D12.Handle;

namespace vt::d3d12
{
	export struct [[nodiscard]] DescriptorAllocation
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle	   = {};
		unsigned					unit_count = 0;
	};

	export class DescriptorAllocator
	{
	public:
		DescriptorAllocator(ID3D12Device4*			   device,
							D3D12_DESCRIPTOR_HEAP_TYPE type,
							UINT					   descriptor_count,
							bool					   shader_visible) :
			unit_size(device->GetDescriptorHandleIncrementSize(type))
		{
			D3D12_DESCRIPTOR_HEAP_DESC const desc {
				.Type			= type,
				.NumDescriptors = descriptor_count,
				.Flags			= shader_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			};
			ID3D12DescriptorHeap* raw_heap;

			auto result = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&raw_heap));
			heap.reset(raw_heap);
			VT_ENSURE_RESULT(result, "Failed to create D3D12 descriptor heap.");

			free_blocks.emplace_back(get_cpu_heap_start(), descriptor_count);
		}

		DescriptorAllocation allocate(unsigned unit_count)
		{
			auto it = std::find_if(free_blocks.begin(), free_blocks.end(), [=](DescriptorAllocation free_block) {
				return free_block.unit_count >= unit_count;
			});
			VT_ENSURE(it != free_blocks.end(), "Descriptor heap is full.");

			auto handle = it->handle;

			if(it->unit_count == unit_count)
				free_blocks.erase(it);
			else
			{
				it->handle.ptr += unit_count * unit_size;
				it->unit_count -= unit_count;
			}
			return {handle, unit_count};
		}

		void deallocate(DescriptorAllocation const alloc)
		{
			if(alloc.handle.ptr == 0)
				return;

			// Find first free block whose handle is greater than the allocation that is being freed.
			auto const next = std::find_if(free_blocks.begin(), free_blocks.end(), [=](DescriptorAllocation free_block) {
				return free_block.handle.ptr > alloc.handle.ptr;
			});

			if(next != free_blocks.end()) // there is a free block after the allocation
			{
				if(alloc.handle.ptr + alloc.unit_count * unit_size == next->handle.ptr) // allocation spans up to next block
				{
					next->handle.ptr = alloc.handle.ptr;  // adjust free block handle down to freed handle
					next->unit_count += alloc.unit_count; // add freed space

					if(next != free_blocks.begin()) // if next is not the first free block, there might be a block to merge
					{
						auto	 prev		= next - 1;
						auto	 prev_ptr	= prev->handle.ptr;
						unsigned prev_count = prev->unit_count;
						if(prev_ptr + prev_count * unit_size == alloc.handle.ptr) // previous block spans up to allocation
						{
							next->handle.ptr = prev_ptr;	// adjust free block handle down to previous free block handle
							next->unit_count += prev_count; // add space of previous block, remove previous block, then exit
							free_blocks.erase(prev);
						}
					}
					return;
				}
			}

			if(next != free_blocks.begin()) // there is a free block somewhere before the next block
			{
				auto& prev = next[-1];
				if(prev.handle.ptr + prev.unit_count * unit_size == alloc.handle.ptr) // previous block spans up to allocation
				{
					prev.unit_count += alloc.unit_count; // add freed space, then exit
					return;
				}
			}
			free_blocks.emplace(next, alloc); // add standalone free block before the next one
		}

		unsigned get_increment_size() const
		{
			return unit_size;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE get_cpu_heap_start() const
		{
			return heap->GetCPUDescriptorHandleForHeapStart();
		}

		D3D12_GPU_DESCRIPTOR_HANDLE get_gpu_heap_start() const
		{
			return heap->GetGPUDescriptorHandleForHeapStart();
		}

		ID3D12DescriptorHeap* ptr() const
		{
			return heap.get();
		}

	private:
		std::deque<DescriptorAllocation> free_blocks;
		ComUnique<ID3D12DescriptorHeap>	 heap;
		unsigned						 unit_size;
	};
}
