module;
#include <cstdint>
export module vt.Graphics.DescriptorBinding;

import vt.Core.Array;
import vt.Core.Specification;

namespace vt
{
	export enum class DescriptorType : uint8_t {
		Texture,
		ReadWriteTexture,
		Buffer,
		ReadWriteBuffer,
		ByteAddressBuffer,
		StructuredBuffer,
		UniformBuffer,
		DynamicSampler
	};

	export struct DescriptorCount
	{
		Positive<unsigned>		 descriptor_count;
		Explicit<DescriptorType> type;
	};

	export enum class ShaderStage : uint8_t {
		All,
		Vertex,
		Hull,
		Domain,
		Geometry,
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

	export struct DescriptorSetBinding
	{
		Explicit<uint8_t>		 shader_register;
		uint8_t					 space = 0;
		Explicit<DescriptorType> type;
		Positive<unsigned>		 count;
	};

	export struct DescriptorSetLayoutSpecification
	{
		ArrayView<DescriptorSetBinding> bindings;
		ShaderStage						visibility = ShaderStage::All;
	};
}
