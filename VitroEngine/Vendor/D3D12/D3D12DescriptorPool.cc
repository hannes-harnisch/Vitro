module;
#include "Core/Macros.hh"
#include "D3D12API.hh"

#include <vector>
export module vt.D3D12.DescriptorPool;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorPoolBase;
import vt.Graphics.DescriptorSet;
import vt.Graphics.Device;

namespace vt::d3d12
{
	export class D3D12DescriptorPool : public DescriptorPoolBase
	{
	public:
		static constexpr unsigned MaxSimultaneousHeaps = 2;

		D3D12DescriptorPool(Device const& in_device, CSpan<DescriptorPoolSize> pool_sizes)
		{
			D3D12_DESCRIPTOR_HEAP_DESC view_heap_desc {
				.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
				.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
			};
			D3D12_DESCRIPTOR_HEAP_DESC sampler_desc {
				.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
				.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
			};
			for(auto size : pool_sizes)
			{
				if(size.type == DescriptorType::DynamicSampler)
					sampler_desc.NumDescriptors += size.descriptor_count;
				else
					view_heap_desc.NumDescriptors += size.descriptor_count;
			}
			ID3D12DescriptorHeap *raw_view_heap, *raw_sampler_heap;

			auto device = in_device.d3d12.ptr();

			auto result = device->CreateDescriptorHeap(&view_heap_desc, IID_PPV_ARGS(&raw_view_heap));
			gpu_view_heap.reset(raw_view_heap);
			VT_ENSURE_RESULT(result, "Failed to create shader-visible D3D12 descriptor heap for CBV/SRV/UAVs.");

			result = device->CreateDescriptorHeap(&sampler_desc, IID_PPV_ARGS(&raw_sampler_heap));
			gpu_sampler_heap.reset(raw_sampler_heap);
			VT_ENSURE_RESULT(result, "Failed to create shader-visible D3D12 descriptor heap for samplers.");
		}

		std::vector<DescriptorSet> allocate_descriptors() override
		{
			return {};
		}

		FixedList<ID3D12DescriptorHeap*, MaxSimultaneousHeaps> get_shader_visible_heaps() const
		{
			FixedList<ID3D12DescriptorHeap*, MaxSimultaneousHeaps> heaps;

			if(gpu_view_heap)
				heaps.emplace_back(gpu_view_heap.get());
			if(gpu_sampler_heap)
				heaps.emplace_back(gpu_sampler_heap.get());

			return heaps;
		}

	private:
		ComUnique<ID3D12DescriptorHeap> gpu_view_heap;
		ComUnique<ID3D12DescriptorHeap> gpu_sampler_heap;
		ComUnique<ID3D12DescriptorHeap> view_staging_heap;
		ComUnique<ID3D12DescriptorHeap> sampler_staging_heap;
	};
}
