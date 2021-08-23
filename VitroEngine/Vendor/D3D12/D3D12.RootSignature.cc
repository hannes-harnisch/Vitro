module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"
export module Vitro.D3D12.RootSignature;

import Vitro.D3D12.Utils;
import Vitro.Graphics.Device;
import Vitro.Graphics.RootSignatureInfo;

namespace vt::d3d12
{
	constexpr D3D12_SHADER_VISIBILITY convertShaderStage(ShaderStage stage)
	{
		using enum ShaderStage;
		switch(stage)
		{
			case Vertex: return D3D12_SHADER_VISIBILITY_VERTEX;
			case Hull: return D3D12_SHADER_VISIBILITY_HULL;
			case Domain: return D3D12_SHADER_VISIBILITY_DOMAIN;
			case Geometry: return D3D12_SHADER_VISIBILITY_GEOMETRY;
			case Fragment: return D3D12_SHADER_VISIBILITY_PIXEL;
			case Compute: return D3D12_SHADER_VISIBILITY_ALL;
			case RayGen: return D3D12_SHADER_VISIBILITY_ALL;
			case AnyHit: return D3D12_SHADER_VISIBILITY_ALL;
			case ClosestHit: return D3D12_SHADER_VISIBILITY_ALL;
			case Miss: return D3D12_SHADER_VISIBILITY_ALL;
			case Intersection: return D3D12_SHADER_VISIBILITY_ALL;
			case Callable: return D3D12_SHADER_VISIBILITY_ALL;
			case Task: return D3D12_SHADER_VISIBILITY_AMPLIFICATION;
			case Mesh: return D3D12_SHADER_VISIBILITY_MESH;
		}
		vtUnreachable();
	}

	export class RootSignature
	{
	public:
		RootSignature(vt::Device const& device, RootSignatureInfo const& info) : rootSignature(makeRootSignature(device, info))
		{}

		ID3D12RootSignature* get() const
		{
			return rootSignature.get();
		}

	private:
		ComUnique<ID3D12RootSignature> rootSignature;

		static decltype(rootSignature) makeRootSignature(vt::Device const& device, RootSignatureInfo const& info)
		{
			D3D12_VERSIONED_ROOT_SIGNATURE_DESC const desc {
				.Version = D3D_ROOT_SIGNATURE_VERSION_1_1,
				.Desc_1_1 {
					.NumStaticSamplers = 0,
					.pStaticSamplers   = nullptr,
					.Flags			   = D3D12_ROOT_SIGNATURE_FLAG_NONE,
				},
			};
			ID3DBlob *rawBlob, *rawError;

			auto result = device.d3d12.d3d12SerializeVersionedRootSignature(&desc, &rawBlob, &rawError);

			ComUnique<ID3DBlob> blob(rawBlob), error(rawError);
			vtAssertResult(result, "Failed to serialize D3D12 root signature.");

			ID3D12RootSignature* rawRootSignature;
			result = device.d3d12.get()->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
															 IID_PPV_ARGS(&rawRootSignature));
			decltype(rootSignature) freshRootSignature(rawRootSignature);
			vtAssertResult(result, "Failed to create D3D12 root signature.");

			return freshRootSignature;
		}
	};
}
