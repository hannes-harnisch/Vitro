module;
#include "Core/Macros.hh"
export module vt.Graphics.DescriptorSetLayout;

import vt.Core.Array;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DynamicGpuApi;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_PRIMARY.DescriptorSetLayout;
#endif
import vt.VT_GPU_API_MODULE.DescriptorSetLayout;

namespace vt
{
	export using DescriptorSetLayout = ResourceVariant<VT_GPU_API_VARIANT_ARGS(DescriptorSetLayout)>;

	export enum class Filter : unsigned char {
		MinMagMipNearest,
		MinMagNearestMipLinear,
		MinNearestMagLinearMipNearest,
		MinNearestMagMipLinear,
		MinLinearMagMipNearest,
		MinLinearMagNearestMipLinear,
		MinMagLinearMipNearest,
		MinMagMipLinear,
		Anisotropic,
	};

	export enum class AddressMode : unsigned char {
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder,
	};

	export enum class CompareOp : unsigned char {
		Never,
		Less,
		Equal,
		LessOrEqual,
		Greater,
		NotEqual,
		GreaterOrEqual,
		Always,
	};

	export enum class BorderColor : unsigned char {
		Transparent,
		OpaqueBlack,
		OpaqueWhite,
	};

	export struct SamplerInfo
	{
		Filter		filter		   = {};
		AddressMode u_address_mode = {};
		AddressMode v_address_mode = {};
		AddressMode w_address_mode = {};
		bool		enable_compare = false;
		float		mip_lod_bias   = 0;
		unsigned	max_anisotropy = 0;
		float		min_lod		   = 0;
		float		max_lod		   = 0;
		CompareOp	compare_op	   = {};
		BorderColor border_color   = {};
	};

	export struct StaticSamplerInfo
	{
		unsigned char slot		 = 0;
		ShaderStage	  visibility = ShaderStage::All;
		SamplerInfo	  sampler_info;
	};

	export struct RootSignatureInfo
	{
		unsigned char			   push_constants_size_in_32bit_units;
		ShaderStage				   push_constants_visibility;
		CSpan<DescriptorSetLayout> layouts;
		CSpan<StaticSamplerInfo>   static_samplers;
	};
}
