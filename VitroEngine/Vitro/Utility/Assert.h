#pragma once

template<typename T, size_t N> constexpr size_t ArrayCount(T (&)[N])
{
	return N;
}

#if VTR_DEBUG

	#define AssertDebug(condition, message)                                                                            \
		{                                                                                                              \
			if(!(condition))                                                                                           \
				__debugbreak();                                                                                        \
		}
	#define AssertArrayRange(arr, index)                                                                               \
		{                                                                                                              \
			if((index) >= ArrayCount(arr))                                                                             \
				__debugbreak();                                                                                        \
		}
	#define AssertCritical(condition, message) AssertDebug(condition, message)

#else

	#define AssertDebug(condition, message)
	#define AssertArrayRange(arr, index)
	#define AssertCritical(condition, message)                                                                         \
		{                                                                                                              \
			if(!(condition))                                                                                           \
				throw std::runtime_error(message);                                                                     \
		}

#endif
