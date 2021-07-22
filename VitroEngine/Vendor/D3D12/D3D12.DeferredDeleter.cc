module;
#include "D3D12.API.hh"
export module Vitro.D3D12.DeferredDeleter;

import Vitro.Core.ConcurrentQueue;
import Vitro.Graphics.DeferredDeleterBase;

namespace vt::d3d12
{
	export class DeferredDeleter : public DeferredDeleterBase
	{
	public:
		void deleteResources() override
		{
			clearQueue(buffers);
			clearQueue(textures);
			clearQueue(pipelines);
		}

	private:
		template<typename T> static void clearQueue(ReaderWriterQueue<T>& queue)
		{
			while(T* resource = queue.peek())
			{
				resource->d3d12.handle->Release();
				queue.pop();
			}
		}
	};
}
