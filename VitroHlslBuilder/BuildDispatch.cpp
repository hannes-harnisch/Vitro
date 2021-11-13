module;
#include <cstdlib>
#include <filesystem>
#include <format>
#include <future>
#include <optional>
#include <stdexcept>
#include <string_view>
export module vttool.HlslBuilder.BuildDispatch;

import vt.Core.Algorithm;
import vt.Core.FixedList;
import vt.Core.Process;
import vttool.HlslBuilder.InputArgs;
import vttool.HlslBuilder.InvokeFxc;
import vttool.HlslBuilder.VulkanRegisterShiftOffsets;

namespace stdf = std::filesystem;

namespace vt::tool
{
	std::string make_output_path(InputArgs const& args, TargetGpuApi api)
	{
		auto path = stdf::path(args.output_directory) / stdf::path(args.source_file_path).stem();

		auto extension = api == TargetGpuApi::D3D12 ? ".cso" : ".spv";
		return path.string() + extension;
	}

	std::string make_type_parameter(InputArgs const& args)
	{
		auto path_parts = split(args.source_file_path, '.');
		auto type_part	= path_parts.end()[-2];

		const char* type_param_start;
		if(type_part == "vert")
			type_param_start = "vs";
		else if(type_part == "hull")
			type_param_start = "hs";
		else if(type_part == "domn")
			type_param_start = "ds";
		else if(type_part == "frag")
			type_param_start = "ps";
		else if(type_part == "comp")
			type_param_start = "cs";
		else
			throw std::invalid_argument("The shader source file name does not indicate its type correctly.");

		return std::format("{}_{}_{}", type_param_start, args.shader_model.major, args.shader_model.minor);
	}

	Process make_dxc_process(std::string_view input_path, std::string_view output_path, std::string_view type, TargetGpuApi api)
	{
		std::string_view api_define;
		std::string		 extra_args;
		if(api == TargetGpuApi::D3D12)
			api_define = "VT_GPU_API_D3D12";
		else
		{
			api_define = "VT_GPU_API_VULKAN";
			extra_args = std::format("-spirv -fvk-t-shift {} all -fvk-b-shift {} all -fvk-u-shift {} all -fvk-s-shift {} all",
									 T_BINDING_OFFSET, B_BINDING_OFFSET, U_BINDING_OFFSET, S_BINDING_OFFSET);
		}

		size_t env_var_length;
		getenv_s(&env_var_length, nullptr, 0, "VULKAN_SDK");
		std::string vulkan_sdk_path(env_var_length - 1, '\0');
		getenv_s(&env_var_length, vulkan_sdk_path.data(), env_var_length, "VULKAN_SDK");

		auto args = std::format("{}/Bin/dxc {} -O3 /Fo {} /D {} /T {} {}", vulkan_sdk_path, input_path, output_path, api_define,
								type, extra_args);
		return Process(args);
	}

	export void dispatch_build(InputArgs const& args)
	{
		constexpr Version MIN_DXC_SHADER_MODEL = {6, 0};

		FixedList<TargetGpuApi, 2> invocation_apis;
		if(args.target_gpu_api == TargetGpuApi::D3D12Vulkan)
			invocation_apis.assign({TargetGpuApi::D3D12, TargetGpuApi::Vulkan});
		else
			invocation_apis.emplace_back(args.target_gpu_api);

		std::optional<Process> dxc;
		std::future<void>	   fxc;
		for(auto api : invocation_apis)
		{
			auto input_path	 = args.source_file_path;
			auto output_path = make_output_path(args, api);
			auto type		 = make_type_parameter(args);

			if(args.shader_model < MIN_DXC_SHADER_MODEL && api == TargetGpuApi::D3D12)
				fxc = std::async(std::launch::async, invoke_fxc, input_path, output_path, type);
			else
				dxc = make_dxc_process(input_path, output_path, type, api);
		}

		if(dxc)
			dxc->wait();
		if(fxc.valid())
			fxc.wait();
	}
}
