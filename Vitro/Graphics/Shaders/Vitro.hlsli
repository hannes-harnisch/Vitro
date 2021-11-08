//#pragma pack_matrix(row_major)

#if VT_GPU_API_D3D12

	#define PUSH_CONST(TYPE, NAME) ConstantBuffer<TYPE> NAME : register(b0, space0)
	#define INPUT_ATTACHMENT(TYPE)

#elif VT_GPU_API_VULKAN

	#define PUSH_CONST(TYPE, NAME) [[vk::push_constant]] TYPE NAME
	#define INPUT_ATTACHMENT(TYPE)

#else

	#error No macro defined that indicates the target GPU API.

#endif
