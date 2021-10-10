module;
#include "Core/Macros.hpp"
export module vt.Graphics.DescriptorSetLayout;

import vt.Core.Array;
import vt.Core.Specification;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.VT_GPU_API_MODULE.DescriptorSetLayout;

#if VT_DYNAMIC_GPU_API
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.DescriptorSetLayout;
#endif

namespace vt
{
	using PlatformDescriptorSetLayout = ResourceVariant<VT_GPU_API_VARIANT_ARGS(DescriptorSetLayout)>;
	export class DescriptorSetLayout : public PlatformDescriptorSetLayout
	{
		using PlatformDescriptorSetLayout::PlatformDescriptorSetLayout;
	};

	export enum class Filter : uint8_t {
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

	export enum class AddressMode : uint8_t {
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder,
	};

	export enum class CompareOp : uint8_t {
		Never,
		Less,
		Equal,
		LessOrEqual,
		Greater,
		NotEqual,
		GreaterOrEqual,
		Always,
	};

	export enum class BorderColor : uint8_t {
		Transparent,
		OpaqueBlack,
		OpaqueWhite,
	};

	export struct SamplerSpecification
	{
		Explicit<Filter>	  filter;
		Explicit<AddressMode> u_address_mode;
		Explicit<AddressMode> v_address_mode;
		Explicit<AddressMode> w_address_mode;
		bool				  enable_compare = false;
		float				  mip_lod_bias	 = 0.0f;
		unsigned			  max_anisotropy = 0;
		float				  min_lod		 = 0.0f;
		float				  max_lod		 = 0.0f;
		CompareOp			  compare_op	 = CompareOp::Never;
		BorderColor			  border_color	 = BorderColor::Transparent;
	};

	export struct StaticSamplerSpecification
	{
		Explicit<uint8_t>	 shader_register;
		uint8_t				 space		= 0;
		ShaderStage			 visibility = ShaderStage::All;
		SamplerSpecification sampler_spec;
	};

	export struct RootSignatureSpecification
	{
		Positive<uint8_t>					  push_constants_byte_size;
		ShaderStage							  push_constants_visibility = ShaderStage::All;
		ConstSpan<DescriptorSetLayout>		  layouts;
		ConstSpan<StaticSamplerSpecification> static_sampler_specs;
	};
}
