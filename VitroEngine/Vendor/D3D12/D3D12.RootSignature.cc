module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"
export module vt.D3D12.RootSignature;

import vt.D3D12.Utils;
import vt.Graphics.Device;
import vt.Graphics.RootSignatureInfo;

namespace vt::d3d12
{
	constexpr D3D12_SHADER_VISIBILITY convert_shader_stage(ShaderStage stage)
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
		VT_UNREACHABLE();
	}

	export class RootSignature
	{
	public:
		RootSignature(vt::Device const& device, RootSignatureInfo const& info) :
			root_signature(make_root_signature(device, info))
		{}

		ID3D12RootSignature* get() const
		{
			return root_signature.get();
		}

	private:
		ComUnique<ID3D12RootSignature> root_signature;

		static decltype(root_signature) make_root_signature(vt::Device const& device, RootSignatureInfo const& info)
		{
			D3D12_VERSIONED_ROOT_SIGNATURE_DESC const desc {
				.Version = D3D_ROOT_SIGNATURE_VERSION_1_1,
				.Desc_1_1 {
					.NumStaticSamplers = 0,
					.pStaticSamplers   = nullptr,
					.Flags			   = D3D12_ROOT_SIGNATURE_FLAG_NONE,
				},
			};
			ID3DBlob *raw_blob, *raw_error;

			auto result = device.d3d12.d3d12_serialize_versioned_root_signature(&desc, &raw_blob, &raw_error);

			ComUnique<ID3DBlob> blob(raw_blob), error(raw_error);
			VT_ASSERT_RESULT(result, "Failed to serialize D3D12 root signature.");

			ID3D12RootSignature* raw_root_signature;
			result = device.d3d12.get()->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
															 IID_PPV_ARGS(&raw_root_signature));
			decltype(root_signature) fresh_root_signature(raw_root_signature);
			VT_ASSERT_RESULT(result, "Failed to create D3D12 root signature.");

			return fresh_root_signature;
		}
	};
}
