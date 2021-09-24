module;
#include "Core/Macros.hh"
#include "D3D12API.hh"

#include <algorithm>
#include <deque>
export module vt.D3D12.DescriptorAllocator;

import vt.D3D12.Handle;

namespace vt::d3d12
{
	template<bool ShaderVisible> class DescriptorAllocator
	{
		using DescriptorHandle = std::conditional_t<ShaderVisible, D3D12_GPU_DESCRIPTOR_HANDLE, D3D12_CPU_DESCRIPTOR_HANDLE>;

	public:
		DescriptorAllocator(ID3D12Device4* device, D3D12_DESCRIPTOR_HEAP_TYPE type, unsigned descriptor_count) :
			increment_size(device->GetDescriptorHandleIncrementSize(type))
		{
			D3D12_DESCRIPTOR_HEAP_DESC const desc {
				.Type			= type,
				.NumDescriptors = descriptor_count,
				.Flags			= ShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			};
			ID3D12DescriptorHeap* raw_heap;

			auto result = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&raw_heap));
			heap.reset(raw_heap);
			VT_ENSURE_RESULT(result, "Failed to create D3D12 descriptor heap.");

			DescriptorHandle heap_start;
			if constexpr(ShaderVisible)
				heap_start = heap->GetGPUDescriptorHandleForHeapStart();
			else
				heap_start = heap->GetCPUDescriptorHandleForHeapStart();

			free_blocks.emplace_back(heap_start, descriptor_count);
		}

		DescriptorHandle allocate(unsigned unit_count)
		{
			auto it = std::find_if(free_blocks.begin(), free_blocks.end(), [=](FreeBlock block) {
				return block.unit_count >= unit_count;
			});
			VT_ENSURE(it != free_blocks.end(), "Descriptor heap is full.");

			auto handle = it->handle;

			if(it->unit_count == unit_count)
				free_blocks.erase(it);
			else
			{
				it->handle.ptr += unit_count * increment_size;
				it->unit_count -= unit_count;
			}
			return handle;
		}

		void deallocate(DescriptorHandle allocation, unsigned unit_count)
		{
			// TODO
		}

		ID3D12DescriptorHeap* ptr() const
		{
			return heap.get();
		}

	private:
		struct FreeBlock
		{
			DescriptorHandle handle;
			unsigned		 unit_count;
		};
		std::deque<FreeBlock>			free_blocks;
		ComUnique<ID3D12DescriptorHeap> heap;
		unsigned						increment_size;
	};

	export using CpuDescriptorAllocator = DescriptorAllocator<false>;
	export using GpuDescriptorAllocator = DescriptorAllocator<true>;
}
