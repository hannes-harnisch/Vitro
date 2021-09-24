module;
#include "Core/Macros.hh"
#include "D3D12API.hh"

#include <vector>
export module vt.D3D12.DescriptorPool;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.D3D12.DescriptorAllocator;
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

		D3D12DescriptorPool(Device const& in_device, ArrayView<DescriptorCount> descriptor_counts) :
			device(in_device.d3d12.ptr()),
			view_staging_allocator(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, sum_view_counts(descriptor_counts)),
			sampler_staging_allocator(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, sum_sampler_counts(descriptor_counts)),
			gpu_view_allocator(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, MaxCbvSrvUavsOnGpu),
			gpu_sampler_allocator(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, MaxDynamicSamplersOnGpu)
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
		static constexpr unsigned MaxCbvSrvUavsOnGpu	  = 1000000;
		static constexpr unsigned MaxDynamicSamplersOnGpu = 2048;

		ID3D12Device4*		   device;
		CpuDescriptorAllocator view_staging_allocator;
		CpuDescriptorAllocator sampler_staging_allocator;
		GpuDescriptorAllocator gpu_view_allocator;
		GpuDescriptorAllocator gpu_sampler_allocator;

		static unsigned sum_view_counts(ArrayView<DescriptorCount> descriptor_counts)
		{
			unsigned count = 0;

			for(auto size : pool_sizes)
				if(size.type != DescriptorType::DynamicSampler)
					count += size.descriptor_count;

			return count;
		}

		static unsigned sum_sampler_counts(ArrayView<DescriptorCount> descriptor_counts)
		{
			for(auto size : pool_sizes)
				if(size.type == DescriptorType::DynamicSampler)
					return size.descriptor_count;

			return 0;
		}
	};
}
