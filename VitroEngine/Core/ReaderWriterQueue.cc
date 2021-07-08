module;
#include <readerwriterqueue/readerwriterqueue.h>
export module Vitro.Core.ReaderWriterQueue;

namespace vt
{
	export template<typename T> using ReaderWriterQueue = moodycamel::ReaderWriterQueue<T>;
}
