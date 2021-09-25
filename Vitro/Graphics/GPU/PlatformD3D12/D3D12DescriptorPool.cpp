module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <vector>
export module vt.Graphics.D3D12.DescriptorPool;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Graphics.D3D12.DescriptorAllocator;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorPoolBase;
import vt.Graphics.DescriptorSet;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.Device;
import vt.Graphics.RootSignature;

namespace vt::d3d12
{
	export class D3D12DescriptorPool final : public DescriptorPoolBase
	{
	public:
		static constexpr unsigned MaxSimultaneousGpuHeaps = 2;

		D3D12DescriptorPool(Device& in_device, ArrayView<DescriptorCount> descriptor_counts) :
			device(in_device.d3d12.ptr()),
			view_staging_allocator(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, sum_view_counts(descriptor_counts), false),
			sampler_staging_allocator(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, sum_sampler_counts(descriptor_counts), false),
			gpu_view_allocator(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, MaxCbvSrvUavsOnGpu, true),
			gpu_sampler_allocator(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, MaxDynamicSamplersOnGpu, true)
		{}

		std::vector<DescriptorSet> allocate_descriptors(ArrayView<DescriptorSetLayout>, RootSignature const&) override
		{
			return {};
		}

		void update_descriptors() override
		{}

		void reset_descriptors() override
		{}

		FixedList<ID3D12DescriptorHeap*, MaxSimultaneousGpuHeaps> get_shader_visible_heaps() const
		{
			return {gpu_view_allocator.ptr(), gpu_sampler_allocator.ptr()};
		}

	private:
		static constexpr unsigned MaxCbvSrvUavsOnGpu	  = 1000000; // Maximum for resource binding tier 2
		static constexpr unsigned MaxDynamicSamplersOnGpu = 2048;	 // Maximum for resource binding tier 2

		ID3D12Device4*		device;
		DescriptorAllocator view_staging_allocator;
		DescriptorAllocator sampler_staging_allocator;
		DescriptorAllocator gpu_view_allocator;
		DescriptorAllocator gpu_sampler_allocator;

		static unsigned sum_view_counts(ArrayView<DescriptorCount> descriptor_counts)
		{
			unsigned sum = 0;

			for(auto count : descriptor_counts)
				if(count.type != DescriptorType::DynamicSampler)
					sum += count.descriptor_count;

			return sum;
		}

		static unsigned sum_sampler_counts(ArrayView<DescriptorCount> descriptor_counts)
		{
			for(auto count : descriptor_counts)
				if(count.type == DescriptorType::DynamicSampler)
					return count.descriptor_count;

			return 0;
		}
	};
}
