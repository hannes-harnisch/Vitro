export module vt.Graphics.DescriptorBinding;

import vt.Core.Array;

namespace vt
{
	export enum class DescriptorType : unsigned char {
		None,
		Texture,
		ReadWriteTexture,
		Buffer,
		ReadWriteBuffer,
		ByteAddressBuffer,
		StructuredBuffer,
		UniformBuffer,
		DynamicSampler
	};

	export struct DescriptorPoolSize
	{
		unsigned	   descriptor_count = 0;
		DescriptorType type				= {};
	};

	export enum class ShaderStage : unsigned char {
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
		unsigned char  shader_register = 0;
		unsigned char  space		   = 0;
		DescriptorType type			   = DescriptorType::None;
		unsigned	   count		   = 1;
	};

	export struct DescriptorSetLayoutInfo
	{
		CSpan<DescriptorSetBinding> bindings;
		ShaderStage					visibility		 = ShaderStage::All;
		unsigned char				update_frequency = 0;
	};
}
