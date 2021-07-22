module;
#include <concurrentqueue/concurrentqueue.h>
#include <readerwriterqueue/readerwriterqueue.h>
export module Vitro.Core.ConcurrentQueue;

namespace vt
{
	export template<typename T> using ConcurrentQueue = moodycamel::ConcurrentQueue<T>;
	export using moodycamel::ConsumerToken;
	export using moodycamel::ProducerToken;

	export template<typename T> using ReaderWriterQueue = moodycamel::ReaderWriterQueue<T>;
}
