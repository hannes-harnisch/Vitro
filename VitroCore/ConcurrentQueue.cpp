module;
#include <concurrentqueue/concurrentqueue.h>
export module vt.Core.ConcurrentQueue;

namespace vt
{
	export using moodycamel::ConcurrentQueue;
	export using moodycamel::ConsumerToken;
	export using moodycamel::ProducerToken;
}
