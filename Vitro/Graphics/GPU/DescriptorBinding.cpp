module;
#include <cstdint>
export module vt.Graphics.DescriptorBinding;

import vt.Core.Array;
import vt.Core.Specification;

namespace vt
{
	export enum class DescriptorType : uint8_t {
		Sampler,
		Texture,
		RwTexture,
		Buffer,
		RwBuffer,
		UniformBuffer,
		StructuredBuffer,
		RwStructuredBuffer,
		ByteAddressBuffer,
		RwByteAddressBuffer,
		InputAttachment,
	};

	export enum class ShaderStage : uint8_t {
		All,
		Vertex,
		Hull,
		Domain,
		Fragment,
		Compute,
		RayGen,
		AnyHit,
		ClosestHit,
		Miss,
		Intersection,
		Callable,
		Task,
		Mesh,
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

	export struct DescriptorBinding
	{
		Explicit<uint8_t>			shader_register;
		Explicit<DescriptorType>	type;
		Positive<unsigned>			count					  = 1;				  // Ignored if static_sampler_spec is not null.
		ShaderStage					static_sampler_visibility = ShaderStage::All; // Ignored if static_sampler_spec is null.
		SamplerSpecification const* static_sampler_spec		  = nullptr;
	};

	export struct DescriptorSetLayoutSpecification
	{
		ArrayView<DescriptorBinding> bindings;
		ShaderStage					 visibility = ShaderStage::All;
	};
}
