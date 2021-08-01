export module Vitro.Graphics.RenderPassBase;

namespace vt
{
	export struct RenderPassDescription
	{
		static constexpr unsigned MaxColorAttachments = 8;
	};

	export class RenderPassBase
	{
	public:
		virtual ~RenderPassBase() = default;
	};
}
