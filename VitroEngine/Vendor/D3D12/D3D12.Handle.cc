module;
#include "D3D12.API.hh"
export module Vitro.D3D12.Handle;

namespace vt::d3d12
{
	export using BufferPtr		  = ID3D12Resource*;
	export using CommandListPtr	  = ID3D12CommandList*;
	export using PipelinePtr	  = ID3D12PipelineState*;
	export using QueryPoolPtr	  = ID3D12QueryHeap*;
	export using RootSignaturePtr = ID3D12RootSignature*;
	export using TexturePtr		  = ID3D12Resource1*;
}
