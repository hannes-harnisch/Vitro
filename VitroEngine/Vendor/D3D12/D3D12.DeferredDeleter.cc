module;
#include "D3D12.API.hh"

#include <concurrentqueue/concurrentqueue.h>
#include <functional>
#include <ranges>
export module Vitro.D3D12.DeferredDeleter;

import Vitro.Core.ConcurrentQueue;
import Vitro.Graphics.DeferredDeleterBase;

namespace vt::d3d12
{
	export class DeferredDeleter : public DeferredDeleterBase
	{
	public:
		DeferredDeleter() : comObjectsToken(comObjects), miscDeletionsToken(miscDeletions)
		{}

		void submit(BufferHandle buffer) override
		{
			enqueueComObject(buffer.d3d12.handle);
		}

		void submit(TextureHandle texture) override
		{
			enqueueComObject(texture.d3d12.handle);
		}

		void submit(PipelineHandle pipeline) override
		{
			enqueueComObject(pipeline.d3d12.handle);
		}

		void submit(RenderPassHandle renderPass) override
		{
			enqueueMiscellaneous([=] { delete renderPass.d3d12.handle; });
		}

		void submit(RootSignatureHandle rootSignature) override
		{
			enqueueComObject(rootSignature.d3d12.handle);
		}

		void deleteAll() override
		{
			flushQueue(comObjects, comObjectsToken, [](IUnknown* comObject) { comObject->Release(); });
			flushQueue(miscDeletions, miscDeletionsToken, [](auto const& func) { func(); });
		}

	private:
		ConcurrentQueue<IUnknown*> comObjects;
		ConcurrentQueue<std::function<void()>> miscDeletions;
		ConsumerToken comObjectsToken;
		ConsumerToken miscDeletionsToken;

		template<typename T> static void flushQueue(ConcurrentQueue<T>& queue, ConsumerToken& token, auto destroy)
		{
			while(queue.size_approx())
			{
				constexpr unsigned MaxElementsDequeued = 20;
				T elems[MaxElementsDequeued];

				size_t count = queue.try_dequeue_bulk(token, elems, MaxElementsDequeued);
				for(auto&& elem : std::views::take(elems, count))
					destroy(elem);
			}
		}

		void enqueueComObject(IUnknown* handle)
		{
			static thread_local ProducerToken const producerToken(comObjects);
			comObjects.enqueue(producerToken, handle);
		}

		void enqueueMiscellaneous(auto const& func)
		{
			static thread_local ProducerToken const producerToken(miscDeletions);
			miscDeletions.enqueue(producerToken, func);
		}
	};
}
