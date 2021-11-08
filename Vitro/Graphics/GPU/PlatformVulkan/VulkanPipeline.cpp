module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <span>
#include <vector>
export module vt.Graphics.Vulkan.Pipeline;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Core.LookupTable;
import vt.Graphics.PipelineSpecification;
import vt.Graphics.RenderPassSpecification;
import vt.Graphics.Vulkan.Handle;
import vt.Graphics.Vulkan.RenderPass;
import vt.Graphics.Vulkan.Sampler;

namespace vt::vulkan
{
	constexpr inline auto VERTEX_DATA_TYPE_LOOKUP = [] {
		LookupTable<VertexDataType, VkFormat> _;
		using enum VertexDataType;

		_[Position]			   = VK_FORMAT_R32G32B32A32_SFLOAT;
		_[TransformedPosition] = _[Position];
		_[TextureCoordinates]  = _[Position];
		_[Normal]			   = _[Position];
		_[Binormal]			   = _[Position];
		_[Tangent]			   = _[Position];
		_[Color]			   = _[Position];
		_[PointSize]		   = VK_FORMAT_R32_SFLOAT;
		_[BlendWeight]		   = VK_FORMAT_R32_SFLOAT;
		_[BlendIndices]		   = VK_FORMAT_R32_UINT;
		return _;
	}();

	constexpr inline auto PRIMITIVE_TOPOLOGY_LOOKUP = [] {
		LookupTable<PrimitiveTopology, VkPrimitiveTopology> _;
		using enum PrimitiveTopology;

		_[PointList]	 = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		_[LineList]		 = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		_[LineStrip]	 = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		_[TriangleList]	 = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		_[TriangleStrip] = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		_[PatchList]	 = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
		return _;
	}();

	constexpr inline auto FILL_MODE_LOOKUP = [] {
		LookupTable<PolygonFillMode, VkPolygonMode> _;
		using enum PolygonFillMode;

		_[Wireframe] = VK_POLYGON_MODE_LINE;
		_[Solid]	 = VK_POLYGON_MODE_FILL;
		return _;
	}();

	constexpr inline auto CULL_MODE_LOOKUP = [] {
		LookupTable<CullMode, VkCullModeFlags> _;
		using enum CullMode;

		_[None]	 = VK_CULL_MODE_NONE;
		_[Front] = VK_CULL_MODE_FRONT_BIT;
		_[Back]	 = VK_CULL_MODE_BACK_BIT;
		return _;
	}();

	constexpr inline auto WINDING_ORDER_LOOKUP = [] {
		LookupTable<WindingOrder, VkFrontFace> _;
		using enum WindingOrder;

		_[CounterClockwise] = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		_[Clockwise]		= VK_FRONT_FACE_CLOCKWISE;
		return _;
	}();

	constexpr inline auto STENCIL_OP_LOOKUP = [] {
		LookupTable<StencilOp, VkStencilOp> _;
		using enum StencilOp;

		_[Keep]			  = VK_STENCIL_OP_KEEP;
		_[Zero]			  = VK_STENCIL_OP_ZERO;
		_[Replace]		  = VK_STENCIL_OP_REPLACE;
		_[IncrementClamp] = VK_STENCIL_OP_INCREMENT_AND_CLAMP;
		_[DecrementClamp] = VK_STENCIL_OP_DECREMENT_AND_CLAMP;
		_[Invert]		  = VK_STENCIL_OP_INVERT;
		_[IncrementWrap]  = VK_STENCIL_OP_INCREMENT_AND_WRAP;
		_[DecrementWrap]  = VK_STENCIL_OP_DECREMENT_AND_WRAP;
		return _;
	}();

	constexpr inline auto LOGIC_OP_LOOKUP = [] {
		LookupTable<LogicOp, VkLogicOp> _;
		using enum LogicOp;

		_[Clear]		= VK_LOGIC_OP_CLEAR;
		_[Set]			= VK_LOGIC_OP_SET;
		_[Copy]			= VK_LOGIC_OP_COPY;
		_[CopyInverted] = VK_LOGIC_OP_COPY_INVERTED;
		_[NoOp]			= VK_LOGIC_OP_NO_OP;
		_[Invert]		= VK_LOGIC_OP_INVERT;
		_[And]			= VK_LOGIC_OP_AND;
		_[Nand]			= VK_LOGIC_OP_NAND;
		_[Or]			= VK_LOGIC_OP_OR;
		_[Nor]			= VK_LOGIC_OP_NOR;
		_[Xor]			= VK_LOGIC_OP_XOR;
		_[Equivalent]	= VK_LOGIC_OP_EQUIVALENT;
		_[AndReverse]	= VK_LOGIC_OP_AND_REVERSE;
		_[AndInverted]	= VK_LOGIC_OP_AND_INVERTED;
		_[OrReverse]	= VK_LOGIC_OP_OR_REVERSE;
		_[OrInverted]	= VK_LOGIC_OP_OR_INVERTED;
		return _;
	}();

	constexpr inline auto BLEND_FACTOR_LOOKUP = [] {
		LookupTable<BlendFactor, VkBlendFactor> _;
		using enum BlendFactor;

		_[Zero]				= VK_BLEND_FACTOR_ZERO;
		_[One]				= VK_BLEND_FACTOR_ONE;
		_[SrcColor]			= VK_BLEND_FACTOR_SRC_COLOR;
		_[SrcColorInv]		= VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		_[SrcAlpha]			= VK_BLEND_FACTOR_SRC_ALPHA;
		_[SrcAlphaInv]		= VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		_[DstAlpha]			= VK_BLEND_FACTOR_DST_ALPHA;
		_[DstAlphaInv]		= VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		_[DstColor]			= VK_BLEND_FACTOR_DST_COLOR;
		_[DstColorInv]		= VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		_[SrcAlphaSaturate] = VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
		_[Src1Color]		= VK_BLEND_FACTOR_SRC1_COLOR;
		_[Src1ColorInv]		= VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
		_[Src1Alpha]		= VK_BLEND_FACTOR_SRC1_ALPHA;
		_[Src1AlphaInv]		= VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
		return _;
	}();

	constexpr inline auto BLEND_OP_LOOKUP = [] {
		LookupTable<BlendOp, VkBlendOp> _;
		using enum BlendOp;

		_[Add]			   = VK_BLEND_OP_ADD;
		_[Subtract]		   = VK_BLEND_OP_SUBTRACT;
		_[ReverseSubtract] = VK_BLEND_OP_REVERSE_SUBTRACT;
		_[Min]			   = VK_BLEND_OP_MIN;
		_[Max]			   = VK_BLEND_OP_MAX;
		return _;
	}();

	VkStencilOpState convert_stencil_op_state(StencilOpState op_state, uint8_t read_mask, uint8_t write_mask)
	{
		return {
			.failOp		 = STENCIL_OP_LOOKUP[op_state.fail_op],
			.passOp		 = STENCIL_OP_LOOKUP[op_state.pass_op],
			.depthFailOp = STENCIL_OP_LOOKUP[op_state.depth_fail_op],
			.compareOp	 = COMPARE_OP_LOOKUP[op_state.compare_op],
			.compareMask = read_mask,
			.writeMask	 = write_mask,
			.reference	 = 0,
		};
	}

	export struct GraphicsPipelineInfoState
	{
		static constexpr unsigned		MAX_SHADER_STAGES	 = 4;
		static constexpr unsigned		MAX_TOTAL_ATTRIBUTES = MAX_VERTEX_BUFFERS * MAX_VERTEX_ATTRIBUTES;
		static constexpr VkDynamicState DYNAMIC_STATES[] {
			VK_DYNAMIC_STATE_VIEWPORT,			// The number of dynamic states should be as small as possible to maximize
			VK_DYNAMIC_STATE_SCISSOR,			// optimization opportunities in the driver when pipelines are compiled. The
			VK_DYNAMIC_STATE_BLEND_CONSTANTS,	// dynamic states specified here are always dynamic in every pipeline we compile
			VK_DYNAMIC_STATE_DEPTH_BOUNDS,		// for Vulkan. They are the smallest common denominator of dynamic states
			VK_DYNAMIC_STATE_STENCIL_REFERENCE, // between D3D12 and Vulkan and it also makes reacting to certain state changes
		};										// simpler, such as for swap chain resizes.

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
		uint32_t															  subpass_index;

		GraphicsPipelineInfoState(RenderPipelineSpecification const& spec)
		{
			initialize_shader_stages(spec);
			initialize_vertex_input(spec);

			input_assembly = VkPipelineInputAssemblyStateCreateInfo {
				.sType					= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
				.topology				= PRIMITIVE_TOPOLOGY_LOOKUP[spec.primitive_topology],
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
			pipeline_layout = spec.root_signature.vulkan.get_handle();
			render_pass		= spec.render_pass.vulkan.get_handle();
			subpass_index	= spec.subpass_index;
		}

		VkGraphicsPipelineCreateInfo convert() const
		{
			return {
				.sType				 = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
				.stageCount			 = count(shader_stages),
				.pStages			 = shader_stages.data(),
				.pVertexInputState	 = &vertex_input,
				.pInputAssemblyState = &input_assembly,
				.pTessellationState	 = &tessellation,
				.pViewportState		 = &viewport,
				.pRasterizationState = &rasterization,
				.pMultisampleState	 = &multisample,
				.pDepthStencilState	 = &depth_stencil,
				.pColorBlendState	 = &color_blend,
				.pDynamicState		 = &dynamic_state,
				.layout				 = pipeline_layout,
				.renderPass			 = render_pass,
				.subpass			 = subpass_index,
				.basePipelineHandle	 = nullptr,
				.basePipelineIndex	 = 0,
			};
		}

	private:
		void initialize_shader_stages(RenderPipelineSpecification const& spec)
		{
			struct ShaderStage
			{
				VkShaderModule		  shader;
				VkShaderStageFlagBits stage_flag;
			};
			FixedList<ShaderStage, MAX_SHADER_STAGES> stages {
				ShaderStage {spec.vertex_shader.vulkan.get_handle(), VK_SHADER_STAGE_VERTEX_BIT},
			};

			if(spec.hull_shader)
				stages.emplace_back(spec.hull_shader->vulkan.get_handle(), VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);

			if(spec.domain_shader)
				stages.emplace_back(spec.domain_shader->vulkan.get_handle(), VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);

			if(spec.fragment_shader)
				stages.emplace_back(spec.fragment_shader->vulkan.get_handle(), VK_SHADER_STAGE_FRAGMENT_BIT);

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
			uint32_t binding = 0;
			for(auto& buffer_binding : spec.vertex_buffer_bindings)
			{
				uint32_t location = 0;
				uint32_t size	  = 0;
				for(auto attribute : buffer_binding.attributes)
				{
					vertex_attributes.emplace_back(VkVertexInputAttributeDescription {
						.location = location++,
						.binding  = binding,
						.format	  = VERTEX_DATA_TYPE_LOOKUP[attribute.type],
						.offset	  = size,
					});
					size += static_cast<uint32_t>(get_vertex_data_type_size(attribute.type));
				}

				vertex_bindings.emplace_back(VkVertexInputBindingDescription {
					.binding   = binding++,
					.stride	   = size,
					.inputRate = buffer_binding.input_rate == VertexBufferInputRate::PerVertex ? VK_VERTEX_INPUT_RATE_VERTEX
																							   : VK_VERTEX_INPUT_RATE_INSTANCE,
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
			uint32_t viewport_count = count(spec.blend.attachment_states);

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
				.polygonMode			 = FILL_MODE_LOOKUP[spec.rasterizer.fill_mode],
				.cullMode				 = CULL_MODE_LOOKUP[spec.rasterizer.cull_mode],
				.frontFace				 = WINDING_ORDER_LOOKUP[spec.rasterizer.winding_order],
				.depthBiasEnable		 = spec.rasterizer.depth_bias != 0,
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
				.rasterizationSamples  = static_cast<VkSampleCountFlagBits>(spec.multisample.rasterizer_sample_count.get()),
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
				.depthCompareOp		   = COMPARE_OP_LOOKUP[ds.depth_compare_op],
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
					.srcColorBlendFactor = BLEND_FACTOR_LOOKUP[state.src_color_factor],
					.dstColorBlendFactor = BLEND_FACTOR_LOOKUP[state.dst_color_factor],
					.colorBlendOp		 = BLEND_OP_LOOKUP[state.color_op],
					.srcAlphaBlendFactor = BLEND_FACTOR_LOOKUP[state.src_alpha_factor],
					.dstAlphaBlendFactor = BLEND_FACTOR_LOOKUP[state.dst_alpha_factor],
					.alphaBlendOp		 = BLEND_OP_LOOKUP[state.alpha_op],
					.colorWriteMask		 = static_cast<VkColorComponentFlags>(state.write_mask),
				});

			color_blend = VkPipelineColorBlendStateCreateInfo {
				.sType			 = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
				.logicOpEnable	 = spec.blend.enable_logic_op,
				.logicOp		 = LOGIC_OP_LOOKUP[spec.blend.logic_op],
				.attachmentCount = count(color_blend_states),
				.pAttachments	 = color_blend_states.data(),
				.blendConstants	 = {},
			};
		}
	};

	export VkComputePipelineCreateInfo convert_compute_pipeline_spec(ComputePipelineSpecification const& spec)
	{
		return {
			.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
			.stage {
				.sType				 = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.stage				 = VK_SHADER_STAGE_COMPUTE_BIT,
				.module				 = spec.compute_shader.vulkan.get_handle(),
				.pName				 = "main",
				.pSpecializationInfo = nullptr,
			},
			.layout				= spec.root_signature.vulkan.get_handle(),
			.basePipelineHandle = nullptr,
			.basePipelineIndex	= 0,
		};
	}

	class Pipeline
	{
	public:
		Pipeline(VkPipeline pipeline, DeviceApiTable const& owner) : pipeline(pipeline, owner)
		{}

		VkPipeline get_handle() const
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
