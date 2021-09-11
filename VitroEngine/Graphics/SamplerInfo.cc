export module vt.Graphics.SamplerInfo;

import vt.Graphics.DescriptorBinding;

namespace vt
{
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
}
