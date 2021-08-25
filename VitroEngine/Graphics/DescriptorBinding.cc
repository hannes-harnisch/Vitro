export module vt.Graphics.DescriptorBinding;

namespace vt
{
	export enum class DescriptorType : unsigned char {
		Texture,
		StorageBuffer,
		UniformBuffer,
		Sampler,
	};

	export struct DescriptorBinding
	{
		unsigned char  slot	 = 0;
		DescriptorType type	 = {};
		unsigned char  count = 1;
	};
}
