module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.Sampler;

import vt.Core.Vector;
import vt.Graphics.D3D12.DescriptorSetLayout;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.Device;

namespace vt::d3d12
{
	D3D12_FILTER convert_filter(Filter filter, bool enable_compare)
	{
		using enum Filter;
		if(enable_compare)
			switch(filter)
			{
				case MinMagMipNearest: return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
				case MinMagNearestMipLinear: return D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
				case MinNearestMagLinearMipNearest: return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
				case MinNearestMagMipLinear: return D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
				case MinLinearMagMipNearest: return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
				case MinLinearMagNearestMipLinear: return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
				case MinMagLinearMipNearest: return D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
				case MinMagMipLinear: return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
				case Anisotropic: return D3D12_FILTER_COMPARISON_ANISOTROPIC;
			}
		else
			switch(filter)
			{
				case MinMagMipNearest: return D3D12_FILTER_MIN_MAG_MIP_POINT;
				case MinMagNearestMipLinear: return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
				case MinNearestMagLinearMipNearest: return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
				case MinNearestMagMipLinear: return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
				case MinLinearMagMipNearest: return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
				case MinLinearMagNearestMipLinear: return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
				case MinMagLinearMipNearest: return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
				case MinMagMipLinear: return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
				case Anisotropic: return D3D12_FILTER_ANISOTROPIC;
			}
		VT_UNREACHABLE();
	}

	D3D12_TEXTURE_ADDRESS_MODE convert_address_mode(AddressMode mode)
	{
		using enum AddressMode;
		switch(mode)
		{
			case Repeat: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			case MirroredRepeat: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
			case ClampToEdge: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
			case ClampToBorder: return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		}
		VT_UNREACHABLE();
	}

	export D3D12_COMPARISON_FUNC convert_compare_op(CompareOp op)
	{
		using enum CompareOp;
		switch(op)
		{
			case Never: return D3D12_COMPARISON_FUNC_NEVER;
			case Less: return D3D12_COMPARISON_FUNC_LESS;
			case Equal: return D3D12_COMPARISON_FUNC_EQUAL;
			case LessOrEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
			case Greater: return D3D12_COMPARISON_FUNC_GREATER;
			case NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
			case GreaterOrEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
			case Always: return D3D12_COMPARISON_FUNC_ALWAYS;
		}
		VT_UNREACHABLE();
	}

	Float4 convert_border_color(BorderColor color)
	{
		using enum BorderColor;
		switch(color)
		{
			case Transparent: return {0, 0, 0, 0};
			case OpaqueBlack: return {0, 0, 0, 1};
			case OpaqueWhite: return {1, 1, 1, 1};
		}
		VT_UNREACHABLE();
	}

	D3D12_SAMPLER_DESC convert_dynamic_sampler_spec(SamplerSpecification const& spec)
	{
		auto border = convert_border_color(spec.border_color);
		return {
			.Filter			= convert_filter(spec.filter, spec.enable_compare),
			.AddressU		= convert_address_mode(spec.u_address_mode),
			.AddressV		= convert_address_mode(spec.v_address_mode),
			.AddressW		= convert_address_mode(spec.w_address_mode),
			.MipLODBias		= spec.mip_lod_bias,
			.MaxAnisotropy	= spec.max_anisotropy,
			.ComparisonFunc = convert_compare_op(spec.compare_op),
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

	export D3D12_STATIC_SAMPLER_DESC convert_static_sampler_spec(StaticSamplerSpecification const& spec)
	{
		auto desc = convert_dynamic_sampler_spec(spec.sampler_spec);
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
			.ShaderRegister	  = spec.shader_register,
			.RegisterSpace	  = spec.space,
			.ShaderVisibility = convert_shader_stage(spec.visibility),
		};
	}

	export class D3D12Sampler
	{
	public:
		// Unused parameter is kept for compatibility with APIs where samplers are first-class device-created objects.
		D3D12Sampler(Device&, SamplerSpecification const& spec) : desc(convert_dynamic_sampler_spec(spec))
		{}

		D3D12_SAMPLER_DESC const& get_desc() const
		{
			return desc;
		}

	private:
		D3D12_SAMPLER_DESC desc;
	};
}
