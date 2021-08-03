module;
#include "D3D12.API.hh"
export module Vitro.D3D12.Handle;

namespace vt::d3d12
{
	export using BufferPtr		  = ID3D12Resource1*;
	export using CommandListPtr	  = ID3D12CommandList*;
	export using DevicePtr		  = ID3D12Device1*;
	export using PipelinePtr	  = ID3D12PipelineState*;
	export using QueryPoolPtr	  = ID3D12QueryHeap*;
	export using QueuePtr		  = ID3D12CommandQueue*;
	export using RootSignaturePtr = ID3D12RootSignature*;
	export using SamplerPtr		  = D3D12_CPU_DESCRIPTOR_HANDLE;
	export using TexturePtr		  = ID3D12Resource*;
}
