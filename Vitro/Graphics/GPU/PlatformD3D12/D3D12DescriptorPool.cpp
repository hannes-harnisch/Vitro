module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <algorithm>
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

	class DescriptorHeap
	{
	public:
		DescriptorHeap(ID3D12Device4& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT descriptor_count, bool shader_visible) :
			stride(device.GetDescriptorHandleIncrementSize(type))
		{
			D3D12_DESCRIPTOR_HEAP_DESC const desc {
				.Type			= type,
				.NumDescriptors = descriptor_count,
				.Flags			= shader_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			};
			auto result = device.CreateDescriptorHeap(&desc, VT_COM_OUT(heap));
			VT_CHECK_RESULT(result, "Failed to create D3D12 descriptor heap.");

			free_blocks.emplace_back(heap->GetCPUDescriptorHandleForHeapStart(), descriptor_count);

			if(shader_visible)
				gpu_heap_start = heap->GetGPUDescriptorHandleForHeapStart();
			else
				gpu_heap_start = {};
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
			return free_blocks[0].handle;
		}

		// Returns a descriptor with value zero if the heap is not shader-visible.
		D3D12_GPU_DESCRIPTOR_HANDLE get_gpu_heap_start() const
		{
			return gpu_heap_start;
		}

		// Only valid for shader-visible descriptor heaps. Calculates the value of the GPU descriptor equivalent to the given
		// CPU descriptor on this heap.
		D3D12_GPU_DESCRIPTOR_HANDLE mirror_descriptor(D3D12_CPU_DESCRIPTOR_HANDLE descriptor) const
		{
			VT_ASSERT(gpu_heap_start.ptr, "This method cannot validly be called on a non-shader-visible descriptor heap.");
			return {descriptor.ptr - get_cpu_heap_start().ptr + gpu_heap_start.ptr};
		}

		ID3D12DescriptorHeap* get_handle() const
		{
			return heap.get();
		}

	private:
		std::deque<DescriptorAllocation> free_blocks;
		ComUnique<ID3D12DescriptorHeap>	 heap;
		D3D12_GPU_DESCRIPTOR_HANDLE		 gpu_heap_start;
		unsigned						 stride;
	};

	// Acts as an owner for all D3D12 descriptor heaps, so as to easily allocate various types of descriptors from one object
	// instead of having to ask for various types of descriptor heaps.
	export class DescriptorPool
	{
	public:
		DescriptorPool(ID3D12Device4& device) :
			rtv_heap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, MAX_RENDER_TARGET_VIEWS, false),
			dsv_heap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, MAX_DEPTH_STENCIL_VIEWS, false),
			view_staging_heap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, MAX_CBV_SRV_UAVS_ON_GPU, false),
			sampler_staging_heap(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, MAX_DYNAMIC_SAMPLER_ON_GPU, false),
			gpu_view_heap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, MAX_CBV_SRV_UAVS_ON_GPU, true),
			gpu_sampler_heap(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, MAX_DYNAMIC_SAMPLER_ON_GPU, true)
		{
			initialize_render_target_null_descriptor(device);
		}

		DescriptorAllocation allocate_render_target_views(size_t count)
		{
			return rtv_heap.allocate(count);
		}

		DescriptorAllocation allocate_depth_stencil_view()
		{
			return dsv_heap.allocate(1);
		}

		void deallocate_render_target_views(DescriptorAllocation rtvs)
		{
			rtv_heap.deallocate(rtvs);
		}

		void deallocate_depth_stencil_views(DescriptorAllocation dsvs)
		{
			dsv_heap.deallocate(dsvs);
		}

		unsigned get_rtv_stride() const
		{
			return rtv_heap.get_stride();
		}

		unsigned get_dsv_stride() const
		{
			return dsv_heap.get_stride();
		}

		// Returns a render target null descriptor for a 2D render target.
		D3D12_CPU_DESCRIPTOR_HANDLE get_rtv_null_descriptor() const
		{
			// We can just return the start of the heap here, since the null descriptor resides at the first allocation.
			return rtv_heap.get_cpu_heap_start();
		}

		ID3D12DescriptorHeap* get_shader_visible_view_heap() const
		{
			return gpu_view_heap.get_handle();
		}

		ID3D12DescriptorHeap* get_shader_visible_sampler_heap() const
		{
			return gpu_sampler_heap.get_handle();
		}

	private:
		static constexpr unsigned MAX_RENDER_TARGET_VIEWS	 = 120; // Arbitrarily chosen.
		static constexpr unsigned MAX_DEPTH_STENCIL_VIEWS	 = 40;	// Arbitrarily chosen.
		static constexpr unsigned MAX_CBV_SRV_UAVS_ON_GPU	 = D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2;
		static constexpr unsigned MAX_DYNAMIC_SAMPLER_ON_GPU = D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE;

		DescriptorHeap rtv_heap;
		DescriptorHeap dsv_heap;
		DescriptorHeap view_staging_heap;
		DescriptorHeap sampler_staging_heap;
		DescriptorHeap gpu_view_heap;
		DescriptorHeap gpu_sampler_heap;

		void initialize_render_target_null_descriptor(ID3D12Device4& device)
		{
			// Guaranteed to return the start of the heap.
			auto alloc = rtv_heap.allocate(1);

			D3D12_RENDER_TARGET_VIEW_DESC const rtv_desc {
				.Format		   = DXGI_FORMAT_R8G8B8A8_UNORM, // TODO: Should another format be chosen? Unknown is not permitted.
				.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
				.Texture2D {
					.MipSlice	= 0,
					.PlaneSlice = 0,
				},
			};
			device.CreateRenderTargetView(nullptr, &rtv_desc, alloc.handle);
		}
	};
}
