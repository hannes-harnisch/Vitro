module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"

#include <algorithm>
#include <deque>
#include <memory>
export module vt.Graphics.D3D12.DescriptorAllocator;

import vt.Core.PointerProxy;
import vt.Graphics.D3D12.Handle;

export bool operator==(D3D12_CPU_DESCRIPTOR_HANDLE left, D3D12_CPU_DESCRIPTOR_HANDLE right)
{
	return left.ptr == right.ptr;
}

namespace vt::d3d12
{
	ComUnique<ID3D12DescriptorHeap> make_descriptor_heap(ID3D12Device4&				 device,
														 D3D12_DESCRIPTOR_HEAP_TYPE	 type,
														 UINT						 descriptor_count,
														 D3D12_DESCRIPTOR_HEAP_FLAGS flags)
	{
		ComUnique<ID3D12DescriptorHeap> heap;

		D3D12_DESCRIPTOR_HEAP_DESC const desc {
			.Type			= type,
			.NumDescriptors = descriptor_count,
			.Flags			= flags,
		};
		auto result = device.CreateDescriptorHeap(&desc, VT_COM_OUT(heap));
		VT_CHECK_RESULT(result, "Failed to create D3D12 descriptor heap.");

		return heap;
	}

	export class FreeListDescriptorAllocator
	{
		struct FreeBlock
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle = {};
			size_t						units  = 0;
		};

	public:
		FreeListDescriptorAllocator(ID3D12Device4& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT descriptor_count) :
			heap(make_descriptor_heap(device, type, descriptor_count, D3D12_DESCRIPTOR_HEAP_FLAG_NONE)),
			stride(device.GetDescriptorHandleIncrementSize(type))
		{
			cpu_heap_start = heap->GetCPUDescriptorHandleForHeapStart();
			free_blocks.emplace_back(cpu_heap_start, descriptor_count);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE allocate()
		{
			if(free_blocks.empty())
				throw std::runtime_error("Descriptor heap is full.");

			auto& block	 = free_blocks[0];
			auto  handle = block.handle;
			if(block.units == 1)
				free_blocks.pop_front();
			else
			{
				block.handle.ptr += stride;
				block.units--;
			}
			return handle;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE allocate(size_t units)
		{
			auto block = std::find_if(free_blocks.begin(), free_blocks.end(), [=](FreeBlock free_block) {
				return free_block.units >= units;
			});
			if(block == free_blocks.end())
				throw std::runtime_error("Descriptor heap is full.");

			auto handle = block->handle;
			if(block->units == units)
				free_blocks.erase(block);
			else
			{
				block->handle.ptr += units * stride;
				block->units -= units;
			}
			return handle;
		}

		void deallocate(D3D12_CPU_DESCRIPTOR_HANDLE alloc)
		{
			deallocate(alloc, 1);
		}

		void deallocate(D3D12_CPU_DESCRIPTOR_HANDLE alloc, size_t units)
		{
			if(alloc.ptr == 0)
				return;

			// Find first free block whose handle value is greater than the allocation that is being freed.
			auto const next = std::find_if(free_blocks.begin(), free_blocks.end(), [=](FreeBlock free_block) {
				return free_block.handle.ptr > alloc.ptr;
			});

			if(next != free_blocks.end()) // there is a free block after the allocation
			{
				if(alloc.ptr + units * stride == next->handle.ptr) // allocation spans up to next block
				{
					next->handle.ptr = alloc.ptr; // adjust free block handle down to freed handle
					next->units += units;		  // add freed space

					if(next != free_blocks.begin()) // if next is not the first free block, there might be a block to merge
					{
						auto   prev		  = next - 1;
						auto   prev_ptr	  = prev->handle.ptr;
						size_t prev_count = prev->units;
						if(prev_ptr + prev_count * stride == alloc.ptr) // previous block spans up to allocation
						{
							next->handle.ptr = prev_ptr; // adjust free block handle down to previous free block handle
							next->units += prev_count;	 // add space of previous block, remove previous block, then exit
							free_blocks.erase(prev);
						}
					}
					return;
				}
			}

			if(next != free_blocks.begin()) // there is a free block somewhere before the next block
			{
				auto& prev = next[-1];
				if(prev.handle.ptr + prev.units * stride == alloc.ptr) // previous block spans up to allocation
				{
					prev.units += units; // add freed space, then exit
					return;
				}
			}
			free_blocks.emplace(next, alloc); // add standalone free block before the next one
		}

		UINT get_stride() const
		{
			return stride;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE get_cpu_heap_start() const
		{
			return cpu_heap_start;
		}

	private:
		std::deque<FreeBlock>			free_blocks;
		ComUnique<ID3D12DescriptorHeap> heap;
		D3D12_CPU_DESCRIPTOR_HANDLE		cpu_heap_start;
		UINT							stride;
	};

	export class LinearDescriptorAllocator
	{
	public:
		LinearDescriptorAllocator(ID3D12Device4& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT descriptor_count) :
			heap(make_descriptor_heap(device, type, descriptor_count, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)),
			stride(device.GetDescriptorHandleIncrementSize(type))
		{
			start			 = heap->GetGPUDescriptorHandleForHeapStart();
			current_position = start;
			end.ptr			 = start.ptr + descriptor_count * stride;
		}

		D3D12_GPU_DESCRIPTOR_HANDLE allocate(size_t count)
		{
			auto allocation = current_position;
			current_position.ptr += count * stride;

			if(current_position.ptr > end.ptr)
				throw std::runtime_error("Shader visible descriptor heap was exhausted.");

			return allocation;
		}

		void reset()
		{
			current_position = start;
		}

		D3D12_GPU_DESCRIPTOR_HANDLE get_gpu_heap_start() const
		{
			return start;
		}

		ID3D12DescriptorHeap* get_handle() const
		{
			return heap.get();
		}

	private:
		ComUnique<ID3D12DescriptorHeap> heap;
		UINT							stride;
		D3D12_GPU_DESCRIPTOR_HANDLE		start;
		D3D12_GPU_DESCRIPTOR_HANDLE		current_position;
		D3D12_GPU_DESCRIPTOR_HANDLE		end;
	};

	struct CpuDescriptorDeleter
	{
		using pointer = PointerProxy<D3D12_CPU_DESCRIPTOR_HANDLE>;

		FreeListDescriptorAllocator* allocator;

		void operator()(D3D12_CPU_DESCRIPTOR_HANDLE handle) const
		{
			allocator->deallocate(handle);
		}
	};
	export using UniqueCpuDescriptor = std::unique_ptr<D3D12_CPU_DESCRIPTOR_HANDLE, CpuDescriptorDeleter>;
}
