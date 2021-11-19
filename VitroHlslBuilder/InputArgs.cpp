module;
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <span>
#include <string_view>
module vttool.HlslBuilder.InputArgs;

import vt.Core.Version;

namespace stdf = std::filesystem;

namespace vt::tool
{
	void check_empty_cmdline(std::span<std::string_view> args)
	{
		if(!args.empty())
			return;

		std::cout << R"(
Vitro HLSL Builder

USAGE: VitroHlslBuilder MyShader.vert.hlsl [options]

FORMAT: "{1}.{2}.hlsl", where {1} is an arbitrary file base name and {2} is one of
	vert (Vertex shader)
	hull (Hull shader)
	domn (Domain shader)
	frag (Fragment shader)
	comp (Compute shader)

OPTIONS:
	--api	Specify D3D12, Vulkan or D3D12+Vulkan as the target GPU API to emit either a CSO file, a SPV file or both.
	--out	Specifies the directory where the compiled file (with the same file name but proper extension) will be created.
	--sm	Specifies the desired shader model, such as "6_0" for shader model 6.0.
			)";

		std::exit(EXIT_SUCCESS);
	}

	InputArgs::InputArgs(std::span<std::string_view> args)
	{
		check_empty_cmdline(args);
		parse_source_file_path(args[0]);

		args = {args.begin() + 1, args.end()}; // Shift over span by one, since we handled the first argument.
		parse_output_directory(args);
		parse_target_gpu_api(args);
		parse_shader_model(args);
	}

	void InputArgs::parse_source_file_path(std::string_view first_arg)
	{
		stdf::path path(first_arg);

		if(!stdf::exists(path))
			throw std::runtime_error("The specified shader source file could not be found.");

		if(!stdf::is_regular_file(path))
		{
			auto msg = std::format("The specified shader source file path ({}) refers to something that is not a file.",
								   first_arg);
			throw std::runtime_error(msg);
		}

		source_file_path = first_arg;
	}

	void InputArgs::parse_output_directory(std::span<std::string_view> args)
	{
		for(auto arg : args)
			if(arg.starts_with(OUTPUT_DIR_PARAM))
			{
				output_directory = arg.substr(OUTPUT_DIR_PARAM.size());
				return;
			}

		throw std::runtime_error("No output directory specified. Specify it with \"--out=...\" when calling HLSL builder.");
	}

	void InputArgs::parse_target_gpu_api(std::span<std::string_view> args)
	{
		for(auto arg : args)
			if(arg.starts_with(TARGET_GPU_API_PARAM))
			{
				arg = arg.substr(TARGET_GPU_API_PARAM.size());
				if(arg == "D3D12")
					target_gpu_api = TargetGpuApi::D3D12;
				else if(arg == "D3D12+Vulkan")
					target_gpu_api = TargetGpuApi::D3D12Vulkan;
				else if(arg == "Vulkan")
					target_gpu_api = TargetGpuApi::Vulkan;
				else
					throw std::runtime_error("Invalid target GPU API specified.");

				return;
			}

		throw std::runtime_error("No target GPU API specified. Specify it with \"--api=...\" when calling HLSL builder.");
	}

	void InputArgs::parse_shader_model(std::span<std::string_view> args)
	{
		for(auto arg : args)
			if(arg.starts_with(SHADER_MODEL_PARAM))
			{
				std::string str(arg.substr(SHADER_MODEL_PARAM.size()));
				shader_model = Version::from_delimited_string(str + "_0", '_');
				return;
			}

		throw std::runtime_error("No shader model specified. Specify it with \"--sm=...\" when calling HLSL builder.");
	}
}
