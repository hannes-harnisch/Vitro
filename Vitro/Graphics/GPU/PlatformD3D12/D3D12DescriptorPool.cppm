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

		DescriptorPool(ID3D12Device4& device, unsigned gpu_view_descriptor_count, unsigned gpu_sampler_descriptor_count);

		SmallList<DescriptorSet>	make_descriptor_sets(ArrayView<DescriptorSetLayout> set_layouts,
														 unsigned const					dynamic_counts[]);
		D3D12_CPU_DESCRIPTOR_HANDLE allocate_render_target_views(size_t count);
		void						deallocate_render_target_views(D3D12_CPU_DESCRIPTOR_HANDLE rtvs, size_t units);
		D3D12_CPU_DESCRIPTOR_HANDLE allocate_depth_stencil_view();
		void						deallocate_depth_stencil_view(D3D12_CPU_DESCRIPTOR_HANDLE dsv);
		UniqueCpuDescriptor			allocate_cbv_srv_uav();
		UniqueCpuDescriptor			allocate_sampler();
		void						reset_shader_visible_heaps();
		UINT						get_rtv_stride() const;
		UINT						get_dsv_stride() const;
		UINT						get_cbv_srv_uav_stride() const;
		UINT						get_sampler_stride() const;
		D3D12_CPU_DESCRIPTOR_HANDLE mirror_view_descriptor(D3D12_GPU_DESCRIPTOR_HANDLE view_descriptor) const;
		D3D12_CPU_DESCRIPTOR_HANDLE mirror_sampler_descriptor(D3D12_GPU_DESCRIPTOR_HANDLE sampler_descriptor) const;

		// Returns a render target null descriptor for a 2D render target.
		D3D12_CPU_DESCRIPTOR_HANDLE get_rtv_null_descriptor() const;
		ID3D12DescriptorHeap*		get_shader_visible_view_heap() const;
		ID3D12DescriptorHeap*		get_shader_visible_sampler_heap() const;

	private:
		FreeListDescriptorAllocator rtv_heap;
		FreeListDescriptorAllocator dsv_heap;
		FreeListDescriptorAllocator cbv_srv_uav_stage_heap;
		FreeListDescriptorAllocator sampler_stage_heap;
		LinearDescriptorAllocator	cbv_srv_uav_gpu_heap;
		LinearDescriptorAllocator	sampler_gpu_heap;

		void			   initialize_render_target_null_descriptor(ID3D12Device4& device);
		D3D12DescriptorSet make_descriptor_set(D3D12DescriptorSetLayout const& layout, unsigned dynamic_count);
		D3D12DescriptorSet make_descriptor_set_for_root_descriptor(D3D12DescriptorSetLayout const& layout,
																   unsigned						   dynamic_count);
		D3D12DescriptorSet make_descriptor_set_for_descriptor_table(D3D12DescriptorSetLayout const& layout,
																	unsigned						dynamic_count);
	};
}
