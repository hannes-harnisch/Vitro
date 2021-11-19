module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"

#include <algorithm>
module vt.Graphics.D3D12.DescriptorAllocator;

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

	FreeListDescriptorAllocator::FreeListDescriptorAllocator(ID3D12Device4&				device,
															 D3D12_DESCRIPTOR_HEAP_TYPE type,
															 UINT						descriptor_count) :
		heap(make_descriptor_heap(device, type, descriptor_count, D3D12_DESCRIPTOR_HEAP_FLAG_NONE)),
		stride(device.GetDescriptorHandleIncrementSize(type))
	{
		cpu_heap_start = heap->GetCPUDescriptorHandleForHeapStart();
		free_blocks.emplace_back(cpu_heap_start, descriptor_count);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FreeListDescriptorAllocator::allocate()
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

	D3D12_CPU_DESCRIPTOR_HANDLE FreeListDescriptorAllocator::allocate(size_t units)
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

	void FreeListDescriptorAllocator::deallocate(D3D12_CPU_DESCRIPTOR_HANDLE alloc)
	{
		deallocate(alloc, 1);
	}

	void FreeListDescriptorAllocator::deallocate(D3D12_CPU_DESCRIPTOR_HANDLE alloc, size_t units)
	{
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

	UINT FreeListDescriptorAllocator::get_stride() const
	{
		return stride;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE FreeListDescriptorAllocator::get_cpu_heap_start() const
	{
		return cpu_heap_start;
	}

	LinearDescriptorAllocator::LinearDescriptorAllocator(ID3D12Device4&				device,
														 D3D12_DESCRIPTOR_HEAP_TYPE type,
														 UINT						descriptor_count) :
		heap(make_descriptor_heap(device, type, descriptor_count, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)),
		stride(device.GetDescriptorHandleIncrementSize(type))
	{
		gpu_start		 = heap->GetGPUDescriptorHandleForHeapStart();
		current_position = gpu_start;
		end.ptr			 = gpu_start.ptr + descriptor_count * stride;
		cpu_start		 = heap->GetCPUDescriptorHandleForHeapStart();
	}

	D3D12_GPU_DESCRIPTOR_HANDLE LinearDescriptorAllocator::allocate(size_t count)
	{
		auto allocation = current_position;
		current_position.ptr += count * stride;

		if(current_position.ptr > end.ptr)
			throw std::runtime_error("Shader visible descriptor heap was exhausted.");

		return allocation;
	}

	void LinearDescriptorAllocator::reset()
	{
		current_position = gpu_start;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE LinearDescriptorAllocator::mirror(D3D12_GPU_DESCRIPTOR_HANDLE descriptor) const
	{
		return {descriptor.ptr - gpu_start.ptr + cpu_start.ptr};
	}

	ID3D12DescriptorHeap* LinearDescriptorAllocator::get_handle() const
	{
		return heap.get();
	}
}
