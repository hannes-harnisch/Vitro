module;
#include <utility>
export module Vitro.Graphics.Pipeline;

import Vitro.Core.Array;
import Vitro.Graphics.DeferredUnique;
import Vitro.Graphics.Handle;

namespace vt
{
	export enum class PrimitiveTopology {
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip,
	};

	export enum class AttributeInputRate {
		PerVertex,
		PerInstance,
	};

	export struct PipelineInfo
	{
		PrimitiveTopology primitiveTopology = {};

		RootSignatureHandle rootSignature;

		Array<char> vertexShaderBytecode;

		Array<char> fragmentShaderBytecode;
	};

	export class Pipeline
	{
	public:
		Pipeline(DeferredUnique<PipelineHandle> pipeline) : pipeline(std::move(pipeline))
		{}

	private:
		DeferredUnique<PipelineHandle> pipeline;
	};
}
