export module Vitro.Graphics.RootSignatureInfo;

namespace vt
{
	export enum class ShaderStage : unsigned char {
		Vertex,
		Hull,
		Domain,
		Geometry,
		Fragment,
		Compute,
		RayGen,
		AnyHit,
		ClosestHit,
		Miss,
		Intersection,
		Callable,
		Task,
		Mesh,
	};

	export struct RootSignatureInfo
	{};
}
