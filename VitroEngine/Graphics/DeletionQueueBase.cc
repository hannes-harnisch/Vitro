module;
#include "Trace/Assert.hh"

#include <new>
export module Vitro.Graphics.DeletionQueueBase;

import Vitro.Core.ReaderWriterQueue;
import Vitro.Core.Singleton;
import Vitro.Graphics.Handle;

namespace vt
{
	export class DeletionQueueBase : public Singleton<DeletionQueueBase>
	{
	public:
		virtual void deleteResources() = 0;

		void enqueue(BufferHandle const buffer)
		{
			bool success = buffers.emplace(buffer);
			vtAssert(success, "Failed to enqueue buffer for deletion.");
		}

		void enqueue(TextureHandle const texture)
		{
			bool success = textures.emplace(texture);
			vtAssert(success, "Failed to enqueue texture for deletion.");
		}

		void enqueue(PipelineHandle const pipeline)
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
