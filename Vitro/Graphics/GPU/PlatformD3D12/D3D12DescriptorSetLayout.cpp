module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"

#include <algorithm>
#include <atomic>
export module vt.Graphics.D3D12.DescriptorSetLayout;

import vt.Core.Array;
import vt.Core.LookupTable;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.D3D12.Sampler;

namespace vt::d3d12
{
	constexpr inline auto DESCRIPTOR_TYPE_LOOKUP = [] {
		LookupTable<DescriptorType, D3D12_DESCRIPTOR_RANGE_TYPE> _;
		using enum DescriptorType;

		_[Sampler]			   = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		_[Buffer]			   = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		_[Texture]			   = _[Buffer];
		_[StructuredBuffer]	   = _[Buffer];
		_[ByteAddressBuffer]   = _[Buffer];
		_[RwBuffer]			   = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		_[RwTexture]		   = _[RwBuffer];
		_[RwStructuredBuffer]  = _[RwBuffer];
		_[RwByteAddressBuffer] = _[RwBuffer];
		_[InputAttachment]	   = _[RwBuffer];
		_[UniformBuffer]	   = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		return _;
	}();

	export class D3D12DescriptorSetLayout
	{
	public:
		D3D12DescriptorSetLayout(DescriptorSetLayoutSpecification const& spec) :
			id(id_counter++),
			visibility(SHADER_STAGE_LOOKUP[spec.visibility]),
			parameter_type(determine_parameter_type(spec)),
			static_samplers(count_static_samplers(spec)),
			descriptor_table_ranges(determine_descriptor_table_range_count(spec))
		{
			initialize_static_samplers(spec);
			if(is_descriptor_table())
				initialize_descriptor_table_ranges(spec);
			else
				root_descriptor_register = spec.bindings[0].shader_register;
		}

		unsigned get_id() const
		{
			return id;
		}

		D3D12_SHADER_VISIBILITY get_visibility() const
		{
			return visibility;
		}

		bool is_descriptor_table() const
		{
			return parameter_type == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		}

		D3D12_ROOT_PARAMETER_TYPE get_root_parameter_type() const
		{
			return parameter_type;
		}

		ConstSpan<D3D12_STATIC_SAMPLER_DESC> get_static_sampler_descs() const
		{
			return static_samplers;
		}

		ConstSpan<D3D12_DESCRIPTOR_RANGE1> get_descriptor_table_ranges() const
		{
			VT_ASSERT(is_descriptor_table(), "This method should not be called on descriptor set layouts that hold "
											 "root descriptors.");
			return descriptor_table_ranges;
		}

		size_t count_descriptor_table_ranges() const
		{
			VT_ASSERT(is_descriptor_table(), "This method should not be called on descriptor set layouts that hold "
											 "root descriptors.");
			return descriptor_table_ranges.size();
		}

		D3D12_ROOT_PARAMETER1 get_root_descriptor_parameter() const
		{
			VT_ASSERT(!is_descriptor_table(), "This method should not be called on descriptor set layouts that hold "
											  "descriptor tables.");
			return {
				.ParameterType = parameter_type,
				.Descriptor {
					.ShaderRegister = root_descriptor_register,
					.RegisterSpace	= 0, // Not the final value, will be set when creating root signature.
					.Flags			= D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC,
				},
				.ShaderVisibility = visibility,
			};
		}

	private:
		static inline std::atomic_uint id_counter = 0;

		unsigned						 id;
		D3D12_SHADER_VISIBILITY			 visibility;
		D3D12_ROOT_PARAMETER_TYPE		 parameter_type;
		UINT							 root_descriptor_register;
		Array<D3D12_STATIC_SAMPLER_DESC> static_samplers;
		Array<D3D12_DESCRIPTOR_RANGE1>	 descriptor_table_ranges;

		static D3D12_ROOT_PARAMETER_TYPE determine_parameter_type(DescriptorSetLayoutSpecification const& spec)
		{
			// A resource binding is any binding that isn't a sampler.
			auto is_resource_binding = [](DescriptorBinding const& binding) {
				return binding.type != DescriptorType::Sampler;
			};
			auto candidate = std::find_if(spec.bindings.begin(), spec.bindings.end(), is_resource_binding);
			if(candidate == spec.bindings.end()) // If there are only sampler bindings, it can't be a root descriptor.
				return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

			auto second = std::find_if(candidate, spec.bindings.end(), is_resource_binding);
			if(second != spec.bindings.end()) // If there are multiple bindings of any type, it can't be a root descriptor.
				return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

			if(candidate->count != 1) // If a binding specifies multiple descriptors, it can't be a root descriptor.
				return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

			switch(candidate->type) // It can only be a root descriptor if it is one of these types.
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

		static size_t count_static_samplers(DescriptorSetLayoutSpecification const& spec)
		{
			size_t count = 0;

			for(auto& binding : spec.bindings)
				if(binding.static_sampler_spec)
					++count;

			return count;
		}

		size_t determine_descriptor_table_range_count(DescriptorSetLayoutSpecification const& spec) const
		{
			if(is_descriptor_table())
				return spec.bindings.size() - static_samplers.size(); // Every binding that isn't a static sampler is suitable.
			else
				return 0;
		}

		void initialize_static_samplers(DescriptorSetLayoutSpecification const& spec)
		{
			auto sampler = static_samplers.begin();
			for(auto& binding : spec.bindings)
				if(binding.static_sampler_spec)
					*sampler++ = convert_static_sampler_spec(*binding.static_sampler_spec, binding.shader_register,
															 binding.static_sampler_visibility);
		}

		void initialize_descriptor_table_ranges(DescriptorSetLayoutSpecification const& spec)
		{
			auto range = descriptor_table_ranges.begin();
			for(auto& binding : spec.bindings)
			{
				if(binding.static_sampler_spec)
					continue;

				*range++ = D3D12_DESCRIPTOR_RANGE1 {
					.RangeType						   = DESCRIPTOR_TYPE_LOOKUP[binding.type],
					.NumDescriptors					   = binding.count,
					.BaseShaderRegister				   = binding.shader_register,
					.RegisterSpace					   = 0, // Not the final value, will be set when creating root signature.
					.Flags							   = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC,
					.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
				};
			}
		}
	};
}
