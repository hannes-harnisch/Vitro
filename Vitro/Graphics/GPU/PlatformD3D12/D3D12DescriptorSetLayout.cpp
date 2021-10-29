module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <atomic>
#include <memory>
#include <vector>
export module vt.Graphics.D3D12.DescriptorSetLayout;

import vt.Core.Array;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.D3D12.Sampler;

namespace vt::d3d12
{
	D3D12_DESCRIPTOR_RANGE_TYPE convert_descriptor_type(DescriptorType type)
	{
		using enum DescriptorType;
		switch(type)
		{ // clang-format off
			case Sampler:			  return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
			case Texture:
			case Buffer:
			case StructuredBuffer:
			case ByteAddressBuffer:	  return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			case RwTexture:
			case RwBuffer:
			case RwStructuredBuffer:
			case RwByteAddressBuffer:
			case InputAttachment:	  return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			case UniformBuffer:		  return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		} // clang-format on
		VT_UNREACHABLE();
	}

	export class D3D12DescriptorSetLayout
	{
	public:
		D3D12DescriptorSetLayout(DescriptorSetLayoutSpecification const& spec) :
			parameter_type(determine_parameter_type(spec.bindings)), visibility(convert_shader_stage(spec.visibility))
		{
			for(auto& binding : spec.bindings)
				if(binding.static_sampler_spec)
					static_samplers.emplace_back(convert_static_sampler_spec(*binding.static_sampler_spec,
																			 binding.shader_register,
																			 binding.static_sampler_visibility));

			if(holds_descriptor_table())
			{
				std::construct_at(&table_ranges);
				for(auto& binding : spec.bindings)
				{
					if(binding.static_sampler_spec)
						continue;

					table_ranges.emplace_back(D3D12_DESCRIPTOR_RANGE1 {
						.RangeType						   = convert_descriptor_type(binding.type),
						.NumDescriptors					   = binding.count,
						.BaseShaderRegister				   = binding.shader_register,
						.RegisterSpace					   = 0, // will be set during root signature creation
						.Flags							   = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC,
						.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
					});
				}
			}
			else
			{
				auto binding = spec.bindings[0];

				D3D12_ROOT_DESCRIPTOR1 descriptor {
					.ShaderRegister = binding.shader_register,
					.RegisterSpace	= 0, // will be set during root signature creation
					.Flags			= D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC,
				};
				std::construct_at(&root_descriptor, descriptor);
			}
		}

		D3D12DescriptorSetLayout(D3D12DescriptorSetLayout&& that) noexcept :
			id(that.id),
			parameter_type(that.parameter_type),
			visibility(that.visibility),
			static_samplers(std::move(that.static_samplers))
		{
			if(holds_descriptor_table())
				std::construct_at(&table_ranges, std::move(that.table_ranges));
			else
				std::construct_at(&root_descriptor, std::move(that.root_descriptor));
		}

		~D3D12DescriptorSetLayout()
		{
			if(holds_descriptor_table())
				std::destroy_at(&table_ranges);
		}

		D3D12DescriptorSetLayout& operator=(D3D12DescriptorSetLayout&& that) noexcept
		{
			id				= that.id;
			parameter_type	= that.parameter_type;
			visibility		= that.visibility;
			static_samplers = std::move(that.static_samplers);

			if(holds_descriptor_table())
				table_ranges = std::move(that.table_ranges);
			else
				root_descriptor = std::move(that.root_descriptor);

			return *this;
		}

		bool holds_descriptor_table() const
		{
			return parameter_type == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		}

		unsigned get_id() const
		{
			return id;
		}

		D3D12_SHADER_VISIBILITY get_visibility() const
		{
			return visibility;
		}

		ConstSpan<D3D12_STATIC_SAMPLER_DESC> get_static_sampler_descs() const
		{
			return static_samplers;
		}

		ConstSpan<D3D12_DESCRIPTOR_RANGE1> get_descriptor_ranges() const
		{
			VT_ASSERT(holds_descriptor_table(), "This method should not be called on descriptor set layouts that hold "
												"root descriptors.");
			return table_ranges;
		}

		D3D12_ROOT_PARAMETER1 get_root_descriptor_parameter() const
		{
			VT_ASSERT(!holds_descriptor_table(), "This method should not be called on descriptor set layouts that hold "
												 "descriptor tables.");
			return {
				.ParameterType	  = parameter_type,
				.Descriptor		  = root_descriptor,
				.ShaderVisibility = visibility,
			};
		}

	private:
		static inline std::atomic_uint id_counter = 0;

		unsigned							   id = id_counter++;
		D3D12_ROOT_PARAMETER_TYPE			   parameter_type : sizeof(char); // TODO: Replace with enum reflection
		D3D12_SHADER_VISIBILITY				   visibility : sizeof(char);
		std::vector<D3D12_STATIC_SAMPLER_DESC> static_samplers;
		union
		{
			std::vector<D3D12_DESCRIPTOR_RANGE1> table_ranges;
			D3D12_ROOT_DESCRIPTOR1				 root_descriptor;
		};

		static D3D12_ROOT_PARAMETER_TYPE determine_parameter_type(ArrayView<DescriptorBinding> bindings)
		{
			auto is_resource_binding = [](auto& binding) {
				return binding.type != DescriptorType::Sampler;
			};
			auto candidate = std::find_if(bindings.begin(), bindings.end(), is_resource_binding);
			if(candidate == bindings.end())
				return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

			auto second = std::find_if(candidate, bindings.end(), is_resource_binding);
			if(second != bindings.end())
				return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

			if(candidate->count != 1)
				return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

			switch(candidate->type)
			{
				using enum DescriptorType;
				case Buffer:
				case StructuredBuffer:
				case ByteAddressBuffer: return D3D12_ROOT_PARAMETER_TYPE_SRV;
				case UniformBuffer: return D3D12_ROOT_PARAMETER_TYPE_CBV;
				case RwTexture:
				case RwBuffer:
				case RwStructuredBuffer:
				case RwByteAddressBuffer:
				case InputAttachment: return D3D12_ROOT_PARAMETER_TYPE_UAV;
			}
			return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		}
	};
}
