module;
#include "Trace/Assert.hh"

#include <new>
export module Vitro.Graphics.DeferredDeleterBase;

import Vitro.Core.ConcurrentQueue;
import Vitro.Core.Singleton;
import Vitro.Graphics.Handle;

namespace vt
{
	export class DeferredDeleterBase : public Singleton<DeferredDeleterBase>
	{
	public:
		virtual void deleteResources() = 0;

		void enqueue(BufferHandle buffer)
		{
			bool success = buffers.emplace(buffer);
			vtAssert(success, "Failed to enqueue buffer for deletion.");
		}

		void enqueue(TextureHandle texture)
		{
			bool success = textures.emplace(texture);
			vtAssert(success, "Failed to enqueue texture for deletion.");
		}

		void enqueue(PipelineHandle pipeline)
		{
			bool success = pipelines.emplace(pipeline);
			vtAssert(success, "Failed to enqueue pipeline for deletion.");
		}

	protected:
		ReaderWriterQueue<BufferHandle> buffers;
		ReaderWriterQueue<TextureHandle> textures;
		ReaderWriterQueue<PipelineHandle> pipelines;
	};
}
