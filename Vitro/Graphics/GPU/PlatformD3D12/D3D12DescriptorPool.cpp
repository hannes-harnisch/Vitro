module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <algorithm>
#include <array>
#include <deque>
#include <vector>
export module vt.Graphics.D3D12.DescriptorPool;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorSet;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.RootSignature;

namespace vt::d3d12
{
	struct [[nodiscard]] DescriptorAllocation
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle	   = {};
		size_t						unit_count = 0;
	};

	class DescriptorAllocator
	{
	public:
		DescriptorAllocator(ID3D12Device4*			   device,
							D3D12_DESCRIPTOR_HEAP_TYPE type,
							UINT					   descriptor_count,
							bool					   shader_visible) :
			stride(device->GetDescriptorHandleIncrementSize(type))
		{
			D3D12_DESCRIPTOR_HEAP_DESC const desc {
				.Type			= type,
				.NumDescriptors = descriptor_count,
				.Flags			= shader_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			};
			ID3D12DescriptorHeap* unowned_heap;

			auto result = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&unowned_heap));
			heap.reset(unowned_heap);
			VT_ENSURE_RESULT(result, "Failed to create D3D12 descriptor heap.");

			free_blocks.emplace_back(get_cpu_heap_start(), descriptor_count);
		}

		DescriptorAllocation allocate(size_t unit_count)
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
				it->handle.ptr += unit_count * stride;
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
				if(alloc.handle.ptr + alloc.unit_count * stride == next->handle.ptr) // allocation spans up to next block
				{
					next->handle.ptr = alloc.handle.ptr;  // adjust free block handle down to freed handle
					next->unit_count += alloc.unit_count; // add freed space

					if(next != free_blocks.begin()) // if next is not the first free block, there might be a block to merge
					{
						auto   prev		  = next - 1;
						auto   prev_ptr	  = prev->handle.ptr;
						size_t prev_count = prev->unit_count;
						if(prev_ptr + prev_count * stride == alloc.handle.ptr) // previous block spans up to allocation
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
				if(prev.handle.ptr + prev.unit_count * stride == alloc.handle.ptr) // previous block spans up to allocation
				{
					prev.unit_count += alloc.unit_count; // add freed space, then exit
					return;
				}
			}
			free_blocks.emplace(next, alloc); // add standalone free block before the next one
		}

		unsigned get_stride() const
		{
			return stride;
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
		unsigned						 stride;
	};

	export class DescriptorPool
	{
	public:
		DescriptorPool(ID3D12Device4* in_device) :
			device(in_device),
			rtv_allocator(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, MAX_RENDER_TARGET_VIEWS, false),
			dsv_allocator(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, MAX_DEPTH_STENCIL_VIEWS, false),
			view_staging_allocator(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, MAX_CBV_SRV_UAVS_ON_GPU, false),
			sampler_staging_allocator(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, MAX_DYNAMIC_SAMPLER_ON_GPU, false),
			gpu_view_allocator(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, MAX_CBV_SRV_UAVS_ON_GPU, true),
			gpu_sampler_allocator(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, MAX_DYNAMIC_SAMPLER_ON_GPU, true)
		{}

		DescriptorAllocation allocate_render_target_views(size_t count)
		{
			return rtv_allocator.allocate(count);
		}

		DescriptorAllocation allocate_depth_stencil_view()
		{
			return dsv_allocator.allocate(1);
		}

		void deallocate_render_target_views(DescriptorAllocation rtvs)
		{
			rtv_allocator.deallocate(rtvs);
		}

		void deallocate_depth_stencil_views(DescriptorAllocation dsvs)
		{
			dsv_allocator.deallocate(dsvs);
		}

		unsigned get_rtv_stride() const
		{
			return rtv_allocator.get_stride();
		}

		unsigned get_dsv_stride() const
		{
			return dsv_allocator.get_stride();
		}

		std::array<ID3D12DescriptorHeap*, 2> get_shader_visible_heaps()
		{
			return {gpu_view_allocator.ptr(), gpu_sampler_allocator.ptr()};
		}

		ID3D12Device4* get_device()
		{
			return device;
		}

	private:
		static constexpr unsigned MAX_RENDER_TARGET_VIEWS	 = 120;		// Arbitrarily chosen.
		static constexpr unsigned MAX_DEPTH_STENCIL_VIEWS	 = 40;		// Arbitrarily chosen.
		static constexpr unsigned MAX_CBV_SRV_UAVS_ON_GPU	 = 1000000; // Maximum for resource binding tier 2
		static constexpr unsigned MAX_DYNAMIC_SAMPLER_ON_GPU = 2048;	// Maximum for resource binding tier 2

		ID3D12Device4*		device;
		DescriptorAllocator rtv_allocator;
		DescriptorAllocator dsv_allocator;
		DescriptorAllocator view_staging_allocator;
		DescriptorAllocator sampler_staging_allocator;
		DescriptorAllocator gpu_view_allocator;
		DescriptorAllocator gpu_sampler_allocator;
	};
}
