module;
#include "D3D12.API.hh"

#include <concurrentqueue/concurrentqueue.h>
#include <ranges>
export module Vitro.D3D12.DeferredDeleter;

import Vitro.Core.ConcurrentQueue;
import Vitro.Graphics.DeferredDeleterBase;

namespace vt::d3d12
{
	export class DeferredDeleter : public DeferredDeleterBase
	{
	public:
		DeferredDeleter() : comObjectsToken(comObjects), renderPassToken(renderPasses)
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
			static thread_local ProducerToken const producerToken(renderPasses);
			renderPasses.enqueue(producerToken, renderPass.d3d12.handle);
		}

		void submit(RootSignatureHandle rootSignature) override
		{
			enqueueComObject(rootSignature.d3d12.handle);
		}

		void deleteAll() override
		{
			flushQueue(comObjects, comObjectsToken, [](IUnknown* comObject) { comObject->Release(); });
			flushQueue(renderPasses, renderPassToken, [](RenderPass* renderPass) { delete renderPass; });
		}

	private:
		ConcurrentQueue<IUnknown*> comObjects;
		ConcurrentQueue<RenderPass*> renderPasses;
		ConsumerToken comObjectsToken;
		ConsumerToken renderPassToken;

		template<typename T> static void flushQueue(ConcurrentQueue<T>& queue, ConsumerToken& token, auto destroy)
		{
			while(queue.size_approx())
			{
				constexpr unsigned MaxElementsDequeued = 20;
				T elems[MaxElementsDequeued];

				size_t count = queue.try_dequeue_bulk(token, elems, MaxElementsDequeued);
				for(T elem : std::views::take(elems, count))
					destroy(elem);
			}
		}

		void enqueueComObject(IUnknown* handle)
		{
			static thread_local ProducerToken const producerToken(comObjects);
			comObjects.enqueue(producerToken, handle);
		}
	};
}
