export module vt.Graphics.DescriptorBinding;

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
		unsigned char  slot	 = 0;
		DescriptorType type	 = DescriptorType::None;
		unsigned short count = 1;
	};

	export struct RootDescriptorBinding
	{
		unsigned char  slot				= 0;
		unsigned char  update_frequency = 0;
		DescriptorType type				= DescriptorType::None;
		ShaderStage	   visibility		= ShaderStage::All;
	};
}
