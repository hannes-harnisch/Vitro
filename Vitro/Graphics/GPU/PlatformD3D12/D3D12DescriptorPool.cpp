module;
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.DescriptorPool;

import vt.Core.Array;
import vt.Core.SmallList;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorSet;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.D3D12.DescriptorAllocator;
import vt.Graphics.D3D12.DescriptorSet;
import vt.Graphics.D3D12.DescriptorSetLayout;
import vt.Graphics.D3D12.Handle;

namespace vt::d3d12
{
	// Acts as an owner for all D3D12 descriptor heaps, so as to easily allocate various types of descriptors from one object
	// instead of having to ask for various types of descriptor heaps.
	export class DescriptorPool
	{
	public:
		static constexpr unsigned MAX_RENDER_TARGET_VIEWS			 = 120;	   // Arbitrarily chosen.
		static constexpr unsigned MAX_DEPTH_STENCIL_VIEWS			 = 40;	   // Arbitrarily chosen.
		static constexpr unsigned MAX_RECOMMENDED_SRVS_ON_GPU		 = 10000;  // Arbitrarily chosen.
		static constexpr unsigned MAX_CBV_SRV_UAV_DESCRIPTORS_ON_CPU = 100000; // Arbitrarily chosen.

		DescriptorPool(ID3D12Device4& device, unsigned gpu_view_descriptor_count, unsigned gpu_sampler_descriptor_count) :
			rtv_heap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, MAX_RENDER_TARGET_VIEWS),
			dsv_heap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, MAX_DEPTH_STENCIL_VIEWS),
			cbv_srv_uav_stage_heap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, MAX_CBV_SRV_UAV_DESCRIPTORS_ON_CPU),
			sampler_stage_heap(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE),
			cbv_srv_uav_gpu_heap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, gpu_view_descriptor_count),
			sampler_gpu_heap(device, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, gpu_sampler_descriptor_count)
		{
			initialize_render_target_null_descriptor(device);
		}

		SmallList<DescriptorSet> make_descriptor_sets(ArrayView<DescriptorSetLayout> set_layouts,
													  unsigned const				 dynamic_counts[])
		{
			SmallList<DescriptorSet> final_sets;
			final_sets.reserve(set_layouts.size());

			for(auto& set_layout : set_layouts)
				final_sets.emplace_back(make_descriptor_set(set_layout.d3d12, *dynamic_counts++));

			return final_sets;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE allocate_render_target_views(size_t count)
		{
			return rtv_heap.allocate(count);
		}

		void deallocate_render_target_views(D3D12_CPU_DESCRIPTOR_HANDLE rtvs, size_t units)
		{
			if(rtvs.ptr)
				rtv_heap.deallocate(rtvs, units);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE allocate_depth_stencil_view()
		{
			return dsv_heap.allocate();
		}

		void deallocate_depth_stencil_view(D3D12_CPU_DESCRIPTOR_HANDLE dsv)
		{
			if(dsv.ptr)
				dsv_heap.deallocate(dsv);
		}

		UniqueCpuDescriptor allocate_cbv_srv_uav()
		{
			auto handle = cbv_srv_uav_stage_heap.allocate();
			return {handle, {&cbv_srv_uav_stage_heap}};
		}

		UniqueCpuDescriptor allocate_sampler()
		{
			auto handle = sampler_stage_heap.allocate();
			return {handle, {&sampler_stage_heap}};
		}

		void reset_shader_visible_heaps()
		{
			cbv_srv_uav_gpu_heap.reset();
			sampler_gpu_heap.reset();
		}

		UINT get_rtv_stride() const
		{
			return rtv_heap.get_stride();
		}

		UINT get_dsv_stride() const
		{
			return dsv_heap.get_stride();
		}

		UINT get_cbv_srv_uav_stride() const
		{
			return cbv_srv_uav_stage_heap.get_stride();
		}

		UINT get_sampler_stride() const
		{
			return sampler_stage_heap.get_stride();
		}

		D3D12_CPU_DESCRIPTOR_HANDLE mirror_view_descriptor(D3D12_GPU_DESCRIPTOR_HANDLE view_descriptor) const
		{
			return cbv_srv_uav_gpu_heap.mirror(view_descriptor);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE mirror_sampler_descriptor(D3D12_GPU_DESCRIPTOR_HANDLE sampler_descriptor) const
		{
			return sampler_gpu_heap.mirror(sampler_descriptor);
		}

		// Returns a render target null descriptor for a 2D render target.
		D3D12_CPU_DESCRIPTOR_HANDLE get_rtv_null_descriptor() const
		{
			// We can just return the start of the heap here, since the null descriptor resides at the first allocation.
			return rtv_heap.get_cpu_heap_start();
		}

		ID3D12DescriptorHeap* get_shader_visible_view_heap() const
		{
			return cbv_srv_uav_gpu_heap.get_handle();
		}

		ID3D12DescriptorHeap* get_shader_visible_sampler_heap() const
		{
			return sampler_gpu_heap.get_handle();
		}

	private:
		FreeListDescriptorAllocator rtv_heap;
		FreeListDescriptorAllocator dsv_heap;
		FreeListDescriptorAllocator cbv_srv_uav_stage_heap;
		FreeListDescriptorAllocator sampler_stage_heap;
		LinearDescriptorAllocator	cbv_srv_uav_gpu_heap;
		LinearDescriptorAllocator	sampler_gpu_heap;

		static unsigned count_descriptors_to_allocate(ConstSpan<D3D12_DESCRIPTOR_RANGE1> ranges, unsigned dynamic_count)
		{
			unsigned count = 0;
			for(auto& range : ranges)
			{
				if(range.NumDescriptors == UINT_MAX)
					count += dynamic_count;
				else
					count += range.NumDescriptors;
			}
			return count;
		}

		void initialize_render_target_null_descriptor(ID3D12Device4& device)
		{
			auto rtv = rtv_heap.allocate(); // Guaranteed to return the start of the heap because this method should get
											// called right after creating the individual heaps.
			D3D12_RENDER_TARGET_VIEW_DESC const rtv_desc {
				.Format		   = DXGI_FORMAT_R8G8B8A8_UNORM, // TODO: Should another format be chosen? Unknown is not permitted.
				.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
				.Texture2D {
					.MipSlice	= 0,
					.PlaneSlice = 0,
				},
			};
			device.CreateRenderTargetView(nullptr, &rtv_desc, rtv);
		}

		D3D12DescriptorSet make_descriptor_set(D3D12DescriptorSetLayout const& layout, unsigned dynamic_count)
		{
			if(layout.has_root_descriptor())
				return make_descriptor_set_for_root_descriptor(layout, dynamic_count);
			else
				return make_descriptor_set_for_descriptor_table(layout, dynamic_count);
		}

		D3D12DescriptorSet make_descriptor_set_for_root_descriptor(D3D12DescriptorSetLayout const& layout,
																   unsigned						   dynamic_count)
		{
			auto	 sampler_ranges = layout.get_sampler_descriptor_table_ranges();
			unsigned sampler_count	= count_descriptors_to_allocate(sampler_ranges, dynamic_count);

			D3D12_GPU_DESCRIPTOR_HANDLE sampler_table_start {};
			if(sampler_count)
				sampler_table_start = sampler_gpu_heap.allocate(sampler_count);

			return {
				layout.get_id(),
				layout.get_view_root_parameter_type(),
				sampler_table_start,
			};
		}

		D3D12DescriptorSet make_descriptor_set_for_descriptor_table(D3D12DescriptorSetLayout const& layout,
																	unsigned						dynamic_count)
		{
			auto	 sampler_ranges = layout.get_sampler_descriptor_table_ranges();
			unsigned sampler_count	= count_descriptors_to_allocate(sampler_ranges, dynamic_count);

			D3D12_GPU_DESCRIPTOR_HANDLE sampler_table_start {};
			if(sampler_count)
				sampler_table_start = sampler_gpu_heap.allocate(sampler_count);

			auto	 view_ranges = layout.get_view_descriptor_table_ranges();
			unsigned view_count	 = count_descriptors_to_allocate(view_ranges, dynamic_count);

			D3D12_GPU_DESCRIPTOR_HANDLE view_table_start {};
			if(view_count)
				view_table_start = cbv_srv_uav_gpu_heap.allocate(view_count);

			return {
				layout.get_id(),
				view_ranges,
				sampler_table_start,
				view_table_start,
			};
		}
	};
}
