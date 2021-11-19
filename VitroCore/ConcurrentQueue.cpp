module;
#include <concurrentqueue/concurrentqueue.h>
#include <iterator>
#include <ranges>
export module vt.Core.ConcurrentQueue;

import vt.Core.FixedList;

namespace vt
{
	export using moodycamel::ConsumerToken;
	export using moodycamel::ProducerToken;

	export template<typename T> class ConcurrentQueue : public moodycamel::ConcurrentQueue<T>
	{
	public:
		using moodycamel::ConcurrentQueue<T>::ConcurrentQueue;

		template<size_t MAX> void consume(ConsumerToken& token, auto consume_func)
		{
			FixedList<T, MAX> elements;

			auto   inserter = std::back_inserter(elements);
			size_t count	= this->try_dequeue_bulk(token, inserter, MAX);

			for(auto& element : elements | std::views::take(count))
				consume_func(element);
		}
	};
}
