#if VT_GPU_API_D3D12

	#define PushConstants(TYPE, NAME) ConstantBuffer<TYPE> NAME : register(b0, space0)
	#define InputAttachment(TYPE)

#elif VT_GPU_API_VULKAN

	#define PushConstants(TYPE, NAME) [[vk::push_constant]] TYPE NAME
	#define InputAttachment(TYPE)

#else

	#error No macro defined that indicates the target GPU API.

#endif
