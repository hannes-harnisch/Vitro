module;
#include <concurrentqueue/concurrentqueue.h>
export module vt.Core.ConcurrentQueue;

namespace vt
{
	export template<typename T> using ConcurrentQueue = moodycamel::ConcurrentQueue<T>;
	export using moodycamel::ConsumerToken;
	export using moodycamel::ProducerToken;
}
