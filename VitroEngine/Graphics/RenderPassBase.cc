export module Vitro.Graphics.RenderPassBase;

namespace vt
{
	export class RenderPassBase
	{
	public:
		virtual ~RenderPassBase() = default;
	};

	export enum class TextureLayout : unsigned char {
		Undefined,
		ColorAttachment,
		DepthStencilAttachment,
		DepthStencilReadOnly,
		ShaderResource,
		CopySrc,
		CopyDst,
	};

	export struct RenderPassDescription
	{
		static constexpr unsigned MaxColorAttachments = 8;
	};
}
