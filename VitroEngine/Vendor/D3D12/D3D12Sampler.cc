﻿module;
#include "Core/Macros.hh"
#include "D3D12API.hh"
export module vt.D3D12.Sampler;

import vt.Core.Vector;
import vt.D3D12.DescriptorSetLayout;
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

	D3D12_STATIC_BORDER_COLOR convert_to_static_border_color(float const (&desc_border_color)[4])
	{
		Float4 color {
			.r = desc_border_color[0],
			.g = desc_border_color[1],
			.b = desc_border_color[2],
			.a = desc_border_color[3],
		};

		if(color == Float4 {0, 0, 0, 0})
			return D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		else if(color == Float4 {0, 0, 0, 1})
			return D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
		else
			return D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	}

	export class D3D12Sampler
	{
	public:
		// Unused device parameter is kept for compatibility with APIs where samplers are first-class device-created objects.
		D3D12Sampler(Device const&, SamplerInfo const& info) : D3D12Sampler(info)
		{}

		D3D12Sampler(SamplerInfo const& info)
		{
			auto border = convert_border_color(info.border_color);

			desc = {
				.Filter			= convert_filter(info.filter, info.enable_compare),
				.AddressU		= convert_address_mode(info.u_address_mode),
				.AddressV		= convert_address_mode(info.v_address_mode),
				.AddressW		= convert_address_mode(info.w_address_mode),
				.MipLODBias		= info.mip_lod_bias,
				.MaxAnisotropy	= info.max_anisotropy,
				.ComparisonFunc = convert_compare_op(info.compare_op),
				.BorderColor {
					border.r,
					border.g,
					border.b,
					border.a,
				},
				.MinLOD = info.min_lod,
				.MaxLOD = info.max_lod,
			};
		}

		D3D12_SAMPLER_DESC const& get_dynamic_sampler_desc() const
		{
			return desc;
		}

		D3D12_STATIC_SAMPLER_DESC get_static_sampler_desc(UINT		  shader_register,
														  UINT		  register_space,
														  ShaderStage shader_visibility) const
		{
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
				.RegisterSpace	  = register_space,
				.ShaderVisibility = convert_shader_stage(shader_visibility),
			};
		}

	private:
		D3D12_SAMPLER_DESC desc;
	};
}
