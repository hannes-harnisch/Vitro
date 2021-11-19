module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"

#include <fstream>
#include <memory>
#include <ranges>
#include <span>
#include <vector>
module vt.Graphics.D3D12.Device;

import vt.Core.Array;
import vt.Core.SmallList;
import vt.Graphics.D3D12.DescriptorPool;
import vt.Graphics.D3D12.DescriptorSetLayout;
import vt.Graphics.D3D12.RootSignature;
import vt.Graphics.D3D12.SwapChain;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.D3D12.Queue;
import vt.Graphics.AbstractDevice;
import vt.Graphics.Handle;
import vt.Graphics.RingBuffer;

namespace vt::d3d12
{
	ComUnique<ID3D12Device4> make_device(Adapter const& adapter)
	{
		ComUnique<ID3D12Device4> device;

		auto result = D3D12CreateDevice(adapter.d3d12.get(), D3D12Device::MIN_FEATURE_LEVEL, VT_COM_OUT(device));
		VT_CHECK_RESULT(result, "Failed to create D3D12 device.");

		return device;
	}

	SyncToken submit_commands(Queue&								   queue,
							  [[maybe_unused]] D3D12_COMMAND_LIST_TYPE type,
							  ArrayView<CommandListHandle>			   cmds,
							  ConstSpan<SyncToken>					   gpu_wait_tokens)
	{
#if VT_DEBUG
		for(auto list : cmds)
		{
			bool is_right_type = list.d3d12->GetType() == type;
			VT_ASSERT(is_right_type, "The type of this command list does not match the type of queue it was submitted to.");
		}
#endif

		queue.submit(cmds, gpu_wait_tokens);
		return {
			queue.get_fence(),
			queue.signal(),
		};
	}

	D3D12Device::D3D12Device(Adapter const& adapter, IDXGIFactory5& factory) :
		factory(&factory),
		device(make_device(adapter)),
		binding_tier(query_resource_binding_tier()),
		render_queue(*device, D3D12_COMMAND_LIST_TYPE_DIRECT),
		compute_queue(*device, D3D12_COMMAND_LIST_TYPE_COMPUTE),
		copy_queue(*device, D3D12_COMMAND_LIST_TYPE_COPY),
		// Heap-allocated so that it's not error-prone to store pointers to the pool in case the device object is moved.
		descriptor_pool(std::make_unique<DescriptorPool>(*device,
														 get_max_resource_descriptors_per_stage(),
														 get_max_sampler_descriptors_per_stage()))
	{
		ensure_required_features_exist();
		initialize_command_signatures();
		initialize_allocator(adapter);
	}

	CopyCommandList D3D12Device::make_copy_command_list()
	{
		return D3D12CommandList<CommandType::Copy>(*device, *descriptor_pool, nullptr, nullptr, nullptr);
	}

	ComputeCommandList D3D12Device::make_compute_command_list()
	{
		return D3D12CommandList<CommandType::Compute>(*device, *descriptor_pool, dispatch_signature.get(), nullptr, nullptr);
	}

	RenderCommandList D3D12Device::make_render_command_list()
	{
		return D3D12CommandList<CommandType::Render>(*device, *descriptor_pool, dispatch_signature.get(), draw_signature.get(),
													 draw_indexed_signature.get());
	}

	Buffer D3D12Device::make_buffer(BufferSpecification const& spec)
	{
		return {
			D3D12Buffer(spec, *device, *allocator, *descriptor_pool),
			spec,
		};
	}

	Image D3D12Device::make_image(ImageSpecification const& spec)
	{
		return {
			D3D12Image(spec, *device, *allocator, *descriptor_pool),
			spec,
		};
	}

	std::vector<ComputePipeline> D3D12Device::make_compute_pipelines(ArrayView<ComputePipelineSpecification> specs)
	{
		std::vector<ComputePipeline> pipelines;
		pipelines.reserve(specs.size());
		for(auto& spec : specs)
			pipelines.emplace_back(D3D12ComputePipeline(spec, *device));
		return pipelines;
	}

	std::vector<RenderPipeline> D3D12Device::make_render_pipelines(ArrayView<RenderPipelineSpecification> specs)
	{
		std::vector<RenderPipeline> pipelines;
		pipelines.reserve(specs.size());
		for(auto& spec : specs)
			pipelines.emplace_back(D3D12RenderPipeline(spec, *device));
		return pipelines;
	}

	SmallList<DescriptorSet> D3D12Device::make_descriptor_sets(ArrayView<DescriptorSetLayout> set_layouts,
															   unsigned const				  dynamic_counts[])
	{
		return descriptor_pool->make_descriptor_sets(set_layouts, dynamic_counts);
	}

	DescriptorSetLayout D3D12Device::make_descriptor_set_layout(DescriptorSetLayoutSpecification const& spec)
	{
		return D3D12DescriptorSetLayout(spec);
	}

	RenderPass D3D12Device::make_render_pass(RenderPassSpecification const& spec)
	{
		return D3D12RenderPass(spec);
	}

	RootSignature D3D12Device::make_root_signature(RootSignatureSpecification const& spec)
	{
		return D3D12RootSignature(spec, *device);
	}

	Sampler D3D12Device::make_sampler(SamplerSpecification const& spec)
	{
		return D3D12Sampler(spec, *device, descriptor_pool->allocate_sampler());
	}

	Shader D3D12Device::make_shader(char const path[])
	{
		return D3D12Shader(std::ifstream(path, std::ios::binary));
	}

	SwapChain D3D12Device::make_swap_chain(Window& window, uint8_t buffer_count)
	{
		return D3D12SwapChain(render_queue, *factory, window, buffer_count);
	}

	void D3D12Device::update_descriptors(ArrayView<DescriptorUpdate> updates)
	{
		SmallList<D3D12_CPU_DESCRIPTOR_HANDLE> src_samplers;
		SmallList<D3D12_CPU_DESCRIPTOR_HANDLE> sampler_dst_starts;
		sampler_dst_starts.reserve(updates.size());
		SmallList<UINT> sampler_dst_sizes;
		sampler_dst_sizes.reserve(updates.size());

		SmallList<D3D12_CPU_DESCRIPTOR_HANDLE> src_views;
		SmallList<D3D12_CPU_DESCRIPTOR_HANDLE> view_dst_starts;
		view_dst_starts.reserve(updates.size());
		SmallList<UINT> view_dst_sizes;
		view_dst_sizes.reserve(updates.size());

		SmallList<UINT> src_sizes;

		for(auto& update : updates)
		{
			if(update.type == DescriptorType::Sampler)
			{
				for(Sampler const& sampler : update.samplers)
					src_samplers.emplace_back(sampler.d3d12.get_handle());
			}
			else
			{}
		}
		device->CopyDescriptors(count(view_dst_starts), view_dst_starts.data(), view_dst_sizes.data(), count(src_views),
								src_views.data(), src_sizes.data(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		device->CopyDescriptors(count(sampler_dst_starts), sampler_dst_starts.data(), sampler_dst_sizes.data(),
								count(src_samplers), src_samplers.data(), src_sizes.data(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	}

	void D3D12Device::reset_descriptors()
	{
		descriptor_pool->reset_shader_visible_heaps();
	}

	void* D3D12Device::map(Buffer const& buffer)
	{
		return buffer.d3d12.map();
	}

	void* D3D12Device::map(Image const& image)
	{
		return image.d3d12.map();
	}

	void D3D12Device::unmap(Buffer const& buffer)
	{
		buffer.d3d12.unmap();
	}

	void D3D12Device::unmap(Image const& image)
	{
		image.d3d12.unmap();
	}

	SyncToken D3D12Device::submit_render_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncToken> gpu_wait_tokens)
	{
		return submit_commands(render_queue, D3D12_COMMAND_LIST_TYPE_DIRECT, cmds, gpu_wait_tokens);
	}

	SyncToken D3D12Device::submit_compute_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncToken> gpu_wait_tokens)
	{
		return submit_commands(compute_queue, D3D12_COMMAND_LIST_TYPE_COMPUTE, cmds, gpu_wait_tokens);
	}

	SyncToken D3D12Device::submit_copy_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncToken> gpu_wait_tokens)
	{
		return submit_commands(copy_queue, D3D12_COMMAND_LIST_TYPE_COPY, cmds, gpu_wait_tokens);
	}

	SyncToken D3D12Device::submit_for_present(ArrayView<CommandListHandle> cmds,
											  SwapChain&				   swap_chain,
											  ConstSpan<SyncToken>		   gpu_wait_tokens)
	{
		auto token = submit_render_commands(cmds, gpu_wait_tokens);
		swap_chain.d3d12.present(render_queue);
		return token;
	}

	void D3D12Device::wait_for_workload(SyncToken cpu_wait_token)
	{
		wait_for_fence_value(cpu_wait_token.d3d12.fence, cpu_wait_token.d3d12.fence_value);
	}

	void D3D12Device::flush_render_queue()
	{
		render_queue.flush();
	}

	void D3D12Device::flush_compute_queue()
	{
		compute_queue.flush();
	}

	void D3D12Device::flush_copy_queue()
	{
		copy_queue.flush();
	}

	void D3D12Device::flush()
	{
		render_queue.flush();
		compute_queue.flush();
		copy_queue.flush();
	}

	unsigned D3D12Device::get_max_resource_descriptors_per_stage() const
	{
		UINT base_count;

		if(binding_tier == D3D12_RESOURCE_BINDING_TIER_1)
			base_count = D3D12_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT + D3D12_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT +
						 D3D12_UAV_SLOT_COUNT;
		else
			base_count = DescriptorPool::MAX_RECOMMENDED_SRVS_ON_GPU + D3D12_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT +
						 D3D12_UAV_SLOT_COUNT;

		return MAX_FRAMES_IN_FLIGHT * base_count;
	}

	unsigned D3D12Device::get_max_sampler_descriptors_per_stage() const
	{
		if(binding_tier == D3D12_RESOURCE_BINDING_TIER_1)
			return MAX_FRAMES_IN_FLIGHT * D3D12_COMMONSHADER_SAMPLER_SLOT_COUNT;
		else
			return D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE;
	}

	D3D12_RESOURCE_BINDING_TIER D3D12Device::query_resource_binding_tier() const
	{
		D3D12_FEATURE_DATA_D3D12_OPTIONS feature_data;

		auto result = device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &feature_data, sizeof feature_data);
		VT_CHECK_RESULT(result, "Failed to query D3D12 device resource binding tier.");

		return feature_data.ResourceBindingTier;
	}

	RenderTarget D3D12Device::make_platform_render_target(RenderTargetSpecification const& spec)
	{
		return {
			D3D12RenderTarget(spec, *device, *descriptor_pool),
			{spec.width, spec.height},
		};
	}

	RenderTarget D3D12Device::make_platform_render_target(SharedRenderTargetSpecification const& spec,
														  SwapChain const&						 swap_chain,
														  unsigned								 back_buffer_index)
	{
		return {
			D3D12RenderTarget(spec, swap_chain, back_buffer_index, *device, *descriptor_pool),
			swap_chain->get_size(),
		};
	}

	void D3D12Device::recreate_platform_render_target(RenderTarget& render_target, RenderTargetSpecification const& spec)
	{
		render_target.d3d12.recreate(*device, spec);
	}

	void D3D12Device::recreate_platform_render_target(RenderTarget&							 render_target,
													  SharedRenderTargetSpecification const& spec,
													  SwapChain const&						 swap_chain,
													  unsigned								 back_buffer_index)
	{
		render_target.d3d12.recreate(*device, spec, swap_chain, back_buffer_index);
	}

	void D3D12Device::ensure_required_features_exist() const
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE feature {
			.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1,
		};
		auto result = device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &feature, sizeof feature);
		VT_CHECK_RESULT(result, "Failed to check for D3D12 root signature feature support.");
		VT_ENSURE(feature.HighestVersion == D3D_ROOT_SIGNATURE_VERSION_1_1,
				  "The DirectX 12 root signature 1.1 feature isn't available. Try updating your Windows version and your "
				  "graphics drivers. If the problem persists, you may need to switch to a newer GPU.");
	}

	void D3D12Device::initialize_command_signatures()
	{
		D3D12_INDIRECT_ARGUMENT_DESC const dispatch_arg {
			.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH,
		};
		D3D12_COMMAND_SIGNATURE_DESC const dispatch_desc {
			.ByteStride		  = sizeof(D3D12_DISPATCH_ARGUMENTS),
			.NumArgumentDescs = 1,
			.pArgumentDescs	  = &dispatch_arg,
		};
		auto result = device->CreateCommandSignature(&dispatch_desc, nullptr, VT_COM_OUT(dispatch_signature));
		VT_CHECK_RESULT(result, "Failed to create D3D12 command signature for indirect dispatch.");

		D3D12_INDIRECT_ARGUMENT_DESC const draw_arg {
			.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW,
		};
		D3D12_COMMAND_SIGNATURE_DESC const draw_desc {
			.ByteStride		  = sizeof(D3D12_DRAW_ARGUMENTS),
			.NumArgumentDescs = 1,
			.pArgumentDescs	  = &draw_arg,
		};
		result = device->CreateCommandSignature(&draw_desc, nullptr, VT_COM_OUT(draw_signature));
		VT_CHECK_RESULT(result, "Failed to create D3D12 command signature for indirect draw.");

		D3D12_INDIRECT_ARGUMENT_DESC const draw_indexed_arg {
			.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED,
		};
		D3D12_COMMAND_SIGNATURE_DESC const draw_indexed_desc {
			.ByteStride		  = sizeof(D3D12_DRAW_INDEXED_ARGUMENTS),
			.NumArgumentDescs = 1,
			.pArgumentDescs	  = &draw_indexed_arg,
		};
		result = device->CreateCommandSignature(&draw_indexed_desc, nullptr, VT_COM_OUT(draw_indexed_signature));
		VT_CHECK_RESULT(result, "Failed to create D3D12 command signature for indirect indexed draw.");
	}

	void D3D12Device::initialize_allocator(Adapter const& adapter)
	{
		D3D12MA::ALLOCATOR_DESC const desc {
			.Flags				= D3D12MA::ALLOCATOR_FLAG_SINGLETHREADED,
			.pDevice			= device.get(),
			.PreferredBlockSize = 0,
			.pAdapter			= adapter.d3d12.get(),
		};
		auto result = D3D12MA::CreateAllocator(&desc, std::out_ptr(allocator));
		VT_CHECK_RESULT(result, "Failed to create D3D12 GPU allocator.");
	}
}
