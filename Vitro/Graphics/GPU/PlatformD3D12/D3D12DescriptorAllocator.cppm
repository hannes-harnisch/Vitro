module;
#include "D3D12API.hpp"

#include <deque>
#include <memory>
export module vt.Graphics.D3D12.DescriptorAllocator;

import vt.Core.PointerProxy;
import vt.Graphics.D3D12.Handle;

export bool operator==(D3D12_CPU_DESCRIPTOR_HANDLE left, D3D12_CPU_DESCRIPTOR_HANDLE right)
{
	return left.ptr == right.ptr;
}

namespace vt::d3d12
{
	export class FreeListDescriptorAllocator
	{
		struct FreeBlock
		{
			D3D12_CPU_DESCRIPTOR_HANDLE handle = {};
			size_t						units  = 0;
		};

	public:
		FreeListDescriptorAllocator(ID3D12Device4& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT descriptor_count);

		D3D12_CPU_DESCRIPTOR_HANDLE allocate();
		D3D12_CPU_DESCRIPTOR_HANDLE allocate(size_t units);
		void						deallocate(D3D12_CPU_DESCRIPTOR_HANDLE alloc);
		void						deallocate(D3D12_CPU_DESCRIPTOR_HANDLE alloc, size_t units);
		UINT						get_stride() const;
		D3D12_CPU_DESCRIPTOR_HANDLE get_cpu_heap_start() const;

	private:
		std::deque<FreeBlock>			free_blocks;
		ComUnique<ID3D12DescriptorHeap> heap;
		D3D12_CPU_DESCRIPTOR_HANDLE		cpu_heap_start;
		UINT							stride;
	};

	export class LinearDescriptorAllocator
	{
	public:
		LinearDescriptorAllocator(ID3D12Device4& device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT descriptor_count);

		D3D12_GPU_DESCRIPTOR_HANDLE allocate(size_t count);
		void						reset();
		D3D12_CPU_DESCRIPTOR_HANDLE mirror(D3D12_GPU_DESCRIPTOR_HANDLE descriptor) const;
		ID3D12DescriptorHeap*		get_handle() const;

	private:
		ComUnique<ID3D12DescriptorHeap> heap;
		UINT							stride;
		D3D12_GPU_DESCRIPTOR_HANDLE		gpu_start;
		D3D12_GPU_DESCRIPTOR_HANDLE		current_position;
		D3D12_GPU_DESCRIPTOR_HANDLE		end;
		D3D12_CPU_DESCRIPTOR_HANDLE		cpu_start;
	};

	struct CpuDescriptorDeleter
	{
		using pointer = PointerProxy<D3D12_CPU_DESCRIPTOR_HANDLE>;

		FreeListDescriptorAllocator* allocator;

		void operator()(D3D12_CPU_DESCRIPTOR_HANDLE handle) const
		{
			allocator->deallocate(handle);
		}
	};
	export using UniqueCpuDescriptor = std::unique_ptr<D3D12_CPU_DESCRIPTOR_HANDLE, CpuDescriptorDeleter>;
}
