module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <span>
#include <vector>
export module vt.Graphics.Vulkan.Pipeline;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Graphics.PipelineSpecification;
import vt.Graphics.RenderPassSpecification;
import vt.Graphics.Vulkan.Driver;
import vt.Graphics.Vulkan.Sampler;

namespace vt::vulkan
{
	VkFormat convert_vertex_data_type(VertexDataType type)
	{
		using enum VertexDataType;
		switch(type)
		{ // clang-format off
			case Position:
			case TransformedPosition:
			case TextureCoordinates:
			case Normal:
			case Binormal:
			case Tangent:
			case Color:		   return VK_FORMAT_R32G32B32A32_SFLOAT;
			case PointSize:
			case BlendWeight:  return VK_FORMAT_R32_SFLOAT;
			case BlendIndices: return VK_FORMAT_R32_UINT;
		}
		VT_UNREACHABLE();
	}

	VkVertexInputRate convert_input_rate(VertexBufferInputRate rate)
	{
		switch(rate)
		{
			case VertexBufferInputRate::PerVertex:	 return VK_VERTEX_INPUT_RATE_VERTEX;
			case VertexBufferInputRate::PerInstance: return VK_VERTEX_INPUT_RATE_INSTANCE;
		}
		VT_UNREACHABLE();
	}

	VkPrimitiveTopology convert_primitive_topology(PrimitiveTopology topology)
	{
		using enum PrimitiveTopology;
		switch(topology)
		{
			case PointList:		return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			case LineList:		return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			case LineStrip:		return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
			case TriangleList:	return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			case TriangleStrip:	return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			case PatchList:		return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
		}
		VT_UNREACHABLE();
	}

	VkPolygonMode convert_fill_mode(PolygonFillMode mode)
	{
		switch(mode)
		{
			case PolygonFillMode::Wireframe: return VK_POLYGON_MODE_LINE;
			case PolygonFillMode::Solid:	 return VK_POLYGON_MODE_FILL;
		}
		VT_UNREACHABLE();
	}

	VkCullModeFlags convert_cull_mode(CullMode mode)
	{
		using enum CullMode;
		switch(mode)
		{
			case None:	return VK_CULL_MODE_NONE;
			case Front:	return VK_CULL_MODE_FRONT_BIT;
			case Back:	return VK_CULL_MODE_BACK_BIT;
		}
		VT_UNREACHABLE();
	}

	VkFrontFace convert_front_face(FrontFace face)
	{
		switch(face)
		{
			case FrontFace::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
			case FrontFace::Clockwise:		  return VK_FRONT_FACE_CLOCKWISE;
		}
		VT_UNREACHABLE();
	}

	VkSampleCountFlagBits convert_rasterizer_sample_count(uint8_t count)
	{
		switch(count)
		{
			case 1:	 return VK_SAMPLE_COUNT_1_BIT;
			case 2:	 return VK_SAMPLE_COUNT_2_BIT;
			case 4:	 return VK_SAMPLE_COUNT_4_BIT;
			case 8:	 return VK_SAMPLE_COUNT_8_BIT;
			case 16: return VK_SAMPLE_COUNT_16_BIT;
			case 32: return VK_SAMPLE_COUNT_32_BIT;
			case 64: return VK_SAMPLE_COUNT_64_BIT;
		}
		VT_UNREACHABLE();
	}

	VkStencilOp convert_stencil_op(StencilOp op)
	{
		using enum StencilOp;
		switch(op)
		{
			case Keep:			 return VK_STENCIL_OP_KEEP;
			case Zero:			 return VK_STENCIL_OP_ZERO;
			case Replace:		 return VK_STENCIL_OP_REPLACE;
			case IncrementClamp: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
			case DecrementClamp: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
			case Invert:		 return VK_STENCIL_OP_INVERT;
			case IncrementWrap:	 return VK_STENCIL_OP_INCREMENT_AND_WRAP;
			case DecrementWrap:	 return VK_STENCIL_OP_DECREMENT_AND_WRAP;
		}
		VT_UNREACHABLE();
	}

	VkLogicOp convert_logic_op(LogicOp op)
	{
		using enum LogicOp;
		switch(op)
		{
			case Clear:		   return VK_LOGIC_OP_CLEAR;
			case Set:		   return VK_LOGIC_OP_SET;
			case Copy:		   return VK_LOGIC_OP_COPY;
			case CopyInverted: return VK_LOGIC_OP_COPY_INVERTED;
			case NoOp:		   return VK_LOGIC_OP_NO_OP;
			case Invert:	   return VK_LOGIC_OP_INVERT;
			case And:		   return VK_LOGIC_OP_AND;
			case Nand:		   return VK_LOGIC_OP_NAND;
			case Or:		   return VK_LOGIC_OP_OR;
			case Nor:		   return VK_LOGIC_OP_NOR;
			case Xor:		   return VK_LOGIC_OP_XOR;
			case Equivalent:   return VK_LOGIC_OP_EQUIVALENT;
			case AndReverse:   return VK_LOGIC_OP_AND_REVERSE;
			case AndInverted:  return VK_LOGIC_OP_AND_INVERTED;
			case OrReverse:	   return VK_LOGIC_OP_OR_REVERSE;
			case OrInverted:   return VK_LOGIC_OP_OR_INVERTED;
		}
		VT_UNREACHABLE();
	}

	VkBlendFactor convert_blend_factor(BlendFactor factor)
	{
		using enum BlendFactor;
		switch(factor)
		{
			case Zero:			   return VK_BLEND_FACTOR_ZERO;
			case One:			   return VK_BLEND_FACTOR_ONE;
			case SrcColor:		   return VK_BLEND_FACTOR_SRC_COLOR;
			case SrcColorInv:	   return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
			case SrcAlpha:		   return VK_BLEND_FACTOR_SRC_ALPHA;
			case SrcAlphaInv:	   return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			case DstAlpha:		   return VK_BLEND_FACTOR_DST_ALPHA;
			case DstAlphaInv:	   return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
			case DstColor:		   return VK_BLEND_FACTOR_DST_COLOR;
			case DstColorInv:	   return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
			case SrcAlphaSaturate: return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
			case Src1Color:		   return VK_BLEND_FACTOR_SRC1_COLOR;
			case Src1ColorInv:	   return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
			case Src1Alpha:		   return VK_BLEND_FACTOR_SRC1_ALPHA;
			case Src1AlphaInv:	   return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
		}
		VT_UNREACHABLE();
	}

	VkBlendOp convert_blend_op(BlendOp op)
	{
		using enum BlendOp;
		switch(op)
		{
			case Add:			  return VK_BLEND_OP_ADD;
			case Subtract:		  return VK_BLEND_OP_SUBTRACT;
			case ReverseSubtract: return VK_BLEND_OP_REVERSE_SUBTRACT;
			case Min:			  return VK_BLEND_OP_MIN;
			case Max:			  return VK_BLEND_OP_MAX;
		} // clang-format on
		VT_UNREACHABLE();
	}

	VkStencilOpState convert_stencil_op_state(StencilOpState op_state, uint8_t read_mask, uint8_t write_mask)
	{
		return {
			.failOp		 = convert_stencil_op(op_state.fail_op),
			.passOp		 = convert_stencil_op(op_state.pass_op),
			.depthFailOp = convert_stencil_op(op_state.depth_fail_op),
			.compareOp	 = convert_compare_op(op_state.compare_op),
			.compareMask = read_mask,
			.writeMask	 = write_mask,
			.reference	 = 0,
		};
	}

	export struct GraphicsPipelineInfoState
	{
		static constexpr unsigned		MAX_SHADER_STAGES	 = 4;
		static constexpr unsigned		MAX_TOTAL_ATTRIBUTES = MAX_VERTEX_BUFFERS * MAX_VERTEX_ATTRIBUTES;
		static constexpr VkDynamicState DYNAMIC_STATES[]	 = {
			VK_DYNAMIC_STATE_VIEWPORT,	   VK_DYNAMIC_STATE_SCISSOR,		   VK_DYNAMIC_STATE_BLEND_CONSTANTS,
			VK_DYNAMIC_STATE_DEPTH_BOUNDS, VK_DYNAMIC_STATE_STENCIL_REFERENCE,
		};

		FixedList<UniqueVkShaderModule, MAX_SHADER_STAGES>					  shader_modules;
		FixedList<VkPipelineShaderStageCreateInfo, MAX_SHADER_STAGES>		  shader_stages;
		FixedList<VkVertexInputBindingDescription, MAX_VERTEX_BUFFERS>		  vertex_bindings;
		FixedList<VkVertexInputAttributeDescription, MAX_TOTAL_ATTRIBUTES>	  vertex_attributes;
		VkPipelineVertexInputStateCreateInfo								  vertex_input;
		VkPipelineInputAssemblyStateCreateInfo								  input_assembly;
		VkPipelineTessellationStateCreateInfo								  tessellation;
		VkPipelineViewportStateCreateInfo									  viewport;
		VkPipelineRasterizationStateCreateInfo								  rasterization;
		VkSampleMask														  sample_masks[2];
		VkPipelineMultisampleStateCreateInfo								  multisample;
		VkPipelineDepthStencilStateCreateInfo								  depth_stencil;
		FixedList<VkPipelineColorBlendAttachmentState, MAX_COLOR_ATTACHMENTS> color_blend_states;
		VkPipelineColorBlendStateCreateInfo									  color_blend;
		VkPipelineDynamicStateCreateInfo									  dynamic_state;
		VkPipelineLayout													  pipeline_layout;
		VkRenderPass														  render_pass;
		unsigned															  subpass_index;

		GraphicsPipelineInfoState(RenderPipelineSpecification const& spec, DeviceApiTable const& api)
		{
			initialize_shader_stages(spec, api);
			initialize_vertex_input(spec);

			input_assembly = VkPipelineInputAssemblyStateCreateInfo {
				.sType					= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
				.topology				= convert_primitive_topology(spec.primitive_topology),
				.primitiveRestartEnable = spec.enable_primitive_restart,
			};
			tessellation = VkPipelineTessellationStateCreateInfo {
				.sType				= VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
				.patchControlPoints = spec.patch_list_control_point_count,
			};

			initialize_viewport(spec);
			initialize_rasterization(spec);
			initialize_multisample(spec);
			initialize_depth_stencil(spec);
			initialize_color_blend(spec);

			dynamic_state = VkPipelineDynamicStateCreateInfo {
				.sType			   = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
				.dynamicStateCount = count(DYNAMIC_STATES),
				.pDynamicStates	   = DYNAMIC_STATES,
			};
			pipeline_layout = spec.root_signature.vulkan.ptr();
			render_pass		= spec.render_pass.vulkan.ptr();
			subpass_index	= spec.subpass_index;
		}

	private:
		void initialize_shader_stages(RenderPipelineSpecification const& spec, DeviceApiTable const& api)
		{
			struct ShaderStage
			{
				VkShaderModule		  shader;
				VkShaderStageFlagBits stage_flag;
			};
			FixedList<ShaderStage, MAX_SHADER_STAGES> stages {
				ShaderStage {spec.vertex_shader.vulkan.ptr(), VK_SHADER_STAGE_VERTEX_BIT},
			};

			if(spec.hull_shader)
				stages.emplace_back(spec.hull_shader->vulkan.ptr(), VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);

			if(spec.domain_shader)
				stages.emplace_back(spec.domain_shader->vulkan.ptr(), VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);

			if(spec.fragment_shader)
				stages.emplace_back(spec.fragment_shader->vulkan.ptr(), VK_SHADER_STAGE_FRAGMENT_BIT);

			for(auto& stage : stages)
				shader_stages.emplace_back(VkPipelineShaderStageCreateInfo {
					.sType				 = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage				 = stage.stage_flag,
					.module				 = stage.shader,
					.pName				 = "main",
					.pSpecializationInfo = nullptr,
				});
		}

		void initialize_vertex_input(RenderPipelineSpecification const& spec)
		{
			unsigned binding = 0;
			for(auto& buffer_binding : spec.vertex_buffer_bindings)
			{
				unsigned location = 0;
				unsigned size	  = 0;
				for(auto attribute : buffer_binding.attributes)
				{
					vertex_attributes.emplace_back(VkVertexInputAttributeDescription {
						.location = location++,
						.binding  = binding,
						.format	  = convert_vertex_data_type(attribute.type),
						.offset	  = size,
					});
					size += static_cast<unsigned>(get_vertex_data_type_size(attribute.type));
				}

				vertex_bindings.emplace_back(VkVertexInputBindingDescription {
					.binding   = binding++,
					.stride	   = size,
					.inputRate = convert_input_rate(buffer_binding.input_rate),
				});
			}

			vertex_input = VkPipelineVertexInputStateCreateInfo {
				.sType							 = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
				.vertexBindingDescriptionCount	 = count(vertex_bindings),
				.pVertexBindingDescriptions		 = vertex_bindings.data(),
				.vertexAttributeDescriptionCount = count(vertex_attributes),
				.pVertexAttributeDescriptions	 = vertex_attributes.data(),
			};
		}

		void initialize_viewport(RenderPipelineSpecification const& spec)
		{
			unsigned viewport_count = count(spec.blend.attachment_states);

			viewport = VkPipelineViewportStateCreateInfo {
				.sType		   = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
				.viewportCount = viewport_count,
				.pViewports	   = nullptr,
				.scissorCount  = viewport_count,
				.pScissors	   = nullptr,
			};
		}

		void initialize_rasterization(RenderPipelineSpecification const& spec)
		{
			rasterization = VkPipelineRasterizationStateCreateInfo {
				.sType					 = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
				.depthClampEnable		 = spec.rasterizer.enable_depth_clip,
				.polygonMode			 = convert_fill_mode(spec.rasterizer.fill_mode),
				.cullMode				 = convert_cull_mode(spec.rasterizer.cull_mode),
				.frontFace				 = convert_front_face(spec.rasterizer.front_face),
				.depthBiasEnable		 = true,
				.depthBiasConstantFactor = static_cast<float>(spec.rasterizer.depth_bias),
				.depthBiasClamp			 = spec.rasterizer.depth_bias_clamp,
				.depthBiasSlopeFactor	 = spec.rasterizer.depth_bias_slope,
				.lineWidth				 = 1.0f,
			};
		}

		void initialize_multisample(RenderPipelineSpecification const& spec)
		{
			sample_masks[0] = sample_masks[1] = spec.multisample.sample_mask;

			multisample = VkPipelineMultisampleStateCreateInfo {
				.sType				   = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
				.rasterizationSamples  = convert_rasterizer_sample_count(spec.multisample.rasterizer_sample_count),
				.sampleShadingEnable   = false,
				.minSampleShading	   = 0.0f,
				.pSampleMask		   = sample_masks,
				.alphaToCoverageEnable = spec.multisample.enable_alpha_to_coverage,
				.alphaToOneEnable	   = false,
			};
		}

		void initialize_depth_stencil(RenderPipelineSpecification const& spec)
		{
			auto ds = spec.depth_stencil;

			depth_stencil = VkPipelineDepthStencilStateCreateInfo {
				.sType				   = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
				.depthTestEnable	   = ds.enable_depth_test,
				.depthWriteEnable	   = ds.enable_depth_write,
				.depthCompareOp		   = convert_compare_op(ds.depth_compare_op),
				.depthBoundsTestEnable = ds.enable_depth_bounds_test,
				.stencilTestEnable	   = ds.enable_stencil_test,
				.front				   = convert_stencil_op_state(ds.front, ds.stencil_read_mask, ds.stencil_write_mask),
				.back				   = convert_stencil_op_state(ds.back, ds.stencil_read_mask, ds.stencil_write_mask),
				.minDepthBounds		   = 0.0f,
				.maxDepthBounds		   = 1.0f, // TODO: test whether this needs to be set despite dynamic state
			};
		}

		void initialize_color_blend(RenderPipelineSpecification const& spec)
		{
			for(auto state : spec.blend.attachment_states)
				color_blend_states.emplace_back(VkPipelineColorBlendAttachmentState {
					.blendEnable		 = state.enable_blend,
					.srcColorBlendFactor = convert_blend_factor(state.src_color_factor),
					.dstColorBlendFactor = convert_blend_factor(state.dst_color_factor),
					.colorBlendOp		 = convert_blend_op(state.color_op),
					.srcAlphaBlendFactor = convert_blend_factor(state.src_alpha_factor),
					.dstAlphaBlendFactor = convert_blend_factor(state.dst_alpha_factor),
					.alphaBlendOp		 = convert_blend_op(state.alpha_op),
					.colorWriteMask		 = static_cast<VkColorComponentFlags>(state.write_mask),
				});

			color_blend = VkPipelineColorBlendStateCreateInfo {
				.sType			 = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
				.logicOpEnable	 = spec.blend.enable_logic_op,
				.logicOp		 = convert_logic_op(spec.blend.logic_op),
				.attachmentCount = count(color_blend_states),
				.pAttachments	 = color_blend_states.data(),
				.blendConstants	 = {},
			};
		}
	};

	class Pipeline
	{
	public:
		Pipeline(VkPipeline pipeline, DeviceApiTable const& owner) : pipeline(pipeline, owner)
		{}

		VkPipeline ptr() const
		{
			return pipeline.get();
		}

	protected:
		UniqueVkPipeline pipeline;
	};

	export class VulkanRenderPipeline : public Pipeline
	{
		using Pipeline::Pipeline;
	};

	export class VulkanComputePipeline : public Pipeline
	{
		using Pipeline::Pipeline;
	};
}
