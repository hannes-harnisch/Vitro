module;
#include <vector>
export module vt.Graphics.TextureInfo;

import vt.Core.FixedList;

namespace vt
{
	export enum class ImageFormat : unsigned char {
		Unknown,
		R32_G32_B32_A32_Float,
		R32_G32_B32_A32_UInt,
		R32_G32_B32_A32_SInt,
		R32_G32_B32_Float,
		R32_G32_B32_UInt,
		R32_G32_B32_SInt,
		R16_G16_B16_A16_Float,
		R16_G16_B16_A16_UNorm,
		R16_G16_B16_A16_UInt,
		R16_G16_B16_A16_SNorm,
		R16_G16_B16_A16_SInt,
		R32_G32_Float,
		R32_G32_UInt,
		R32_G32_SInt,
		R32_G8_X24_Typeless,
		D32_Float_S8_X24_UInt,
		R10_G10_B10_A2_UNorm,
		R10_G10_B10_A2_UInt,
		R11_G11_B10_Float,
		R8_G8_B8_A8_UNorm,
		R8_G8_B8_A8_UNormSrgb,
		R8_G8_B8_A8_UInt,
		R8_G8_B8_A8_SNorm,
		R8_G8_B8_A8_SInt,
		B8_G8_R8_A8_UNorm,
		B8_G8_R8_A8_UNormSrgb,
		R16_G16_Float,
		R16_G16_UNorm,
		R16_G16_UInt,
		R16_G16_SNorm,
		R16_G16_SInt,
		R32_Typeless,
		D32_Float,
		R32_Float,
		R32_UInt,
		R32_SInt,
		R24_G8_Typeless,
		D24_UNorm_S8_UInt,
		R8_G8_UNorm,
		R8_G8_UInt,
		R8_G8_SNorm,
		R8_G8_SInt,
		R16_Typeless,
		R16_Float,
		D16_UNorm,
		R16_UNorm,
		R16_UInt,
		R16_SNorm,
		R16_SInt,
		R8_UNorm,
		R8_UInt,
		R8_SNorm,
		R8_SInt,
		Bc1_UNorm,
		Bc1_UNormSrgb,
		Bc2_UNorm,
		Bc2_UNormSrgb,
		Bc3_UNorm,
		Bc3_UNormSrgb,
		Bc4_UNorm,
		Bc4_SNorm,
		Bc5_UNorm,
		Bc5_SNorm,
		Bc6H_UFloat16,
		Bc6H_SFloat16,
		Bc7_UNorm,
		Bc7_UNormSrgb,
	};

	export enum class ImageLayout : unsigned char {
		Undefined,
		General,
		CopySource,
		CopyTarget,
		ColorAttachment,
		DepthStencilAttachment,
		DepthStencilReadOnly,
		ShaderResource,
		FragmentShaderResource,
		NonFragmentShaderResource,
		UnorderedAccess,
		Presentable,
	};

	export struct TextureInfo
	{};

	export constexpr unsigned MaxColorAttachments = 8;						 // limit imposed by D3D12 for pipelines
	export constexpr unsigned MaxAttachments	  = MaxColorAttachments + 1; // All color attachments + 1 depth attachment

	export enum class ImageLoadOp : unsigned char {
		Load,
		Clear,
		Ignore,
	};

	export enum class ImageStoreOp : unsigned char {
		Store,
		Ignore,
	};

	export struct AttachmentInfo
	{
		ImageFormat	  format		 = {};
		unsigned char sample_count	 = 1;
		ImageLoadOp	  load_op		 = {};
		ImageStoreOp  store_op		 = {};
		ImageLayout	  initial_layout = {};
		ImageLayout	  final_layout	 = {};
	};

	export struct AttachmentReference
	{
		unsigned char index		  = 0;
		ImageLayout	  used_layout = {};
	};

	export struct Subpass
	{
		FixedList<AttachmentReference, MaxColorAttachments> output_refs;
	};

	export struct RenderPassInfo
	{
		FixedList<AttachmentInfo, MaxAttachments> attachments;
		ImageLoadOp								  stencil_load_op  = {};
		ImageStoreOp							  stencil_store_op = {};
		std::vector<Subpass>					  subpasses;
	};
}
