module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.Sampler;

import vt.Core.LookupTable;
import vt.Core.Vector;
import vt.Graphics.DescriptorBinding;

namespace vt::d3d12
{
	constexpr inline auto COMPARISON_FILTER_LOOKUP = [] {
		LookupTable<Filter, D3D12_FILTER> _;
		using enum Filter;

		_[MinMagMipNearest]				 = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		_[MinMagNearestMipLinear]		 = D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
		_[MinNearestMagLinearMipNearest] = D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
		_[MinNearestMagMipLinear]		 = D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
		_[MinLinearMagMipNearest]		 = D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
		_[MinLinearMagNearestMipLinear]	 = D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		_[MinMagLinearMipNearest]		 = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		_[MinMagMipLinear]				 = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		_[Anisotropic]					 = D3D12_FILTER_COMPARISON_ANISOTROPIC;
		return _;
	}();

	constexpr inline auto FILTER_LOOKUP = [] {
		LookupTable<Filter, D3D12_FILTER> _;
		using enum Filter;

		_[MinMagMipNearest]				 = D3D12_FILTER_MIN_MAG_MIP_POINT;
		_[MinMagNearestMipLinear]		 = D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		_[MinNearestMagLinearMipNearest] = D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		_[MinNearestMagMipLinear]		 = D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		_[MinLinearMagMipNearest]		 = D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		_[MinLinearMagNearestMipLinear]	 = D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		_[MinMagLinearMipNearest]		 = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		_[MinMagMipLinear]				 = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		_[Anisotropic]					 = D3D12_FILTER_ANISOTROPIC;
		return _;
	}();

	D3D12_FILTER convert_filter(Filter filter, bool enable_compare)
	{
		if(enable_compare)
			return COMPARISON_FILTER_LOOKUP[filter];
		else
			return FILTER_LOOKUP[filter];
	}

	constexpr inline auto ADDRESS_MODE_LOOKUP = [] {
		LookupTable<AddressMode, D3D12_TEXTURE_ADDRESS_MODE> _;
		using enum AddressMode;

		_[Repeat]		  = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		_[MirroredRepeat] = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		_[ClampToEdge]	  = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		_[ClampToBorder]  = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		return _;
	}();

	export constexpr inline auto COMPARE_OP_LOOKUP = [] {
		LookupTable<CompareOp, D3D12_COMPARISON_FUNC> _;
		using enum CompareOp;

		_[Never]		  = D3D12_COMPARISON_FUNC_NEVER;
		_[Less]			  = D3D12_COMPARISON_FUNC_LESS;
		_[Equal]		  = D3D12_COMPARISON_FUNC_EQUAL;
		_[LessOrEqual]	  = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		_[Greater]		  = D3D12_COMPARISON_FUNC_GREATER;
		_[NotEqual]		  = D3D12_COMPARISON_FUNC_NOT_EQUAL;
		_[GreaterOrEqual] = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		_[Always]		  = D3D12_COMPARISON_FUNC_ALWAYS;
		return _;
	}();

	constexpr inline auto BORDER_COLOR_LOOKUP = [] {
		LookupTable<BorderColor, Float4> _;
		using enum BorderColor;

		_[Transparent] = {0, 0, 0, 0};
		_[OpaqueBlack] = {0, 0, 0, 1};
		_[OpaqueWhite] = {1, 1, 1, 1};
		return _;
	}();

	export constexpr inline auto SHADER_STAGE_LOOKUP = [] {
		LookupTable<ShaderStage, D3D12_SHADER_VISIBILITY> _;
		using enum ShaderStage;

		_[Vertex]		= D3D12_SHADER_VISIBILITY_VERTEX;
		_[Hull]			= D3D12_SHADER_VISIBILITY_HULL;
		_[Domain]		= D3D12_SHADER_VISIBILITY_DOMAIN;
		_[Fragment]		= D3D12_SHADER_VISIBILITY_PIXEL;
		_[Task]			= D3D12_SHADER_VISIBILITY_AMPLIFICATION;
		_[Mesh]			= D3D12_SHADER_VISIBILITY_MESH;
		_[All]			= D3D12_SHADER_VISIBILITY_ALL;
		_[Compute]		= _[All];
		_[RayGen]		= _[All];
		_[AnyHit]		= _[All];
		_[ClosestHit]	= _[All];
		_[Miss]			= _[All];
		_[Intersection] = _[All];
		_[Callable]		= _[All];
		return _;
	}();

	D3D12_SAMPLER_DESC convert_dynamic_sampler_spec(SamplerSpecification const& spec)
	{
		auto border = BORDER_COLOR_LOOKUP[spec.border_color];
		return {
			.Filter			= convert_filter(spec.filter, spec.enable_compare),
			.AddressU		= ADDRESS_MODE_LOOKUP[spec.u_address_mode],
			.AddressV		= ADDRESS_MODE_LOOKUP[spec.v_address_mode],
			.AddressW		= ADDRESS_MODE_LOOKUP[spec.w_address_mode],
			.MipLODBias		= spec.mip_lod_bias,
			.MaxAnisotropy	= spec.max_anisotropy,
			.ComparisonFunc = COMPARE_OP_LOOKUP[spec.compare_op],
			.BorderColor {
				border.r,
				border.g,
				border.b,
				border.a,
			},
			.MinLOD = spec.min_lod,
			.MaxLOD = spec.max_lod,
		};
	}

	D3D12_STATIC_BORDER_COLOR convert_to_static_border_color(decltype(D3D12_SAMPLER_DESC::BorderColor) const& border_color)
	{
		Float4 color {
			.r = border_color[0],
			.g = border_color[1],
			.b = border_color[2],
			.a = border_color[3],
		};

		if(color == Float4 {0, 0, 0, 0})
			return D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		else if(color == Float4 {0, 0, 0, 1})
			return D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
		else
			return D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	}

	export D3D12_STATIC_SAMPLER_DESC convert_static_sampler_spec(SamplerSpecification const& spec,
																 unsigned					 shader_register,
																 ShaderStage				 visibility)
	{
		auto desc = convert_dynamic_sampler_spec(spec);
		return {
			.Filter			  = desc.Filter,
			.AddressU		  = desc.AddressU,
			.AddressV		  = desc.AddressV,
			.AddressW		  = desc.AddressW,
			.MipLODBias		  = desc.MipLODBias,
			.MaxAnisotropy	  = desc.MaxAnisotropy,
			.ComparisonFunc	  = desc.ComparisonFunc,
			.BorderColor	  = convert_to_static_border_color(desc.BorderColor),
			.MinLOD			  = desc.MinLOD,
			.MaxLOD			  = desc.MaxLOD,
			.ShaderRegister	  = shader_register,
			.RegisterSpace	  = 0, // will be set during root signature creation
			.ShaderVisibility = SHADER_STAGE_LOOKUP[visibility],
		};
	}

	export class D3D12Sampler
	{
	public:
		D3D12Sampler(SamplerSpecification const& spec) : desc(convert_dynamic_sampler_spec(spec))
		{}

		D3D12_SAMPLER_DESC const& get_desc() const
		{
			return desc;
		}

	private:
		D3D12_SAMPLER_DESC desc;
	};
}
