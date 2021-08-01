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
		DeferredDeleter() : conToken(comObjects)
		{}

		void submit(BufferHandle buffer) override
		{
			enqueueComObject(buffer.d3d12());
		}

		void submit(TextureHandle texture) override
		{
			enqueueComObject(texture.d3d12());
		}

		void submit(PipelineHandle pipeline) override
		{
			enqueueComObject(pipeline.d3d12());
		}

		void submit(RootSignatureHandle rootSignature) override
		{
			enqueueComObject(rootSignature.d3d12());
		}

		void deleteAll() override
		{
			while(comObjects.size_approx())
			{
				constexpr unsigned MaxDequeued = 20;

				IUnknown* interfaces[MaxDequeued];
				size_t	  count = comObjects.try_dequeue_bulk(conToken, interfaces, MaxDequeued);
				for(auto comObject : std::views::take(interfaces, count))
					comObject->Release();
			}
		}

	private:
		ConcurrentQueue<IUnknown*> comObjects;
		ConsumerToken			   conToken;

		void enqueueComObject(IUnknown* handle)
		{
			thread_local ProducerToken const proToken(comObjects);
			comObjects.enqueue(proToken, handle);
		}
	};
}
