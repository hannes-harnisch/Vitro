module;
#include <span>
#include <string_view>
export module vttool.HlslBuilder.InputArgs;

import vt.Core.Version;

namespace vt::tool
{
	export enum class TargetGpuApi : uint8_t {
		D3D12,
		D3D12Vulkan,
		Vulkan,
	};

	export class InputArgs
	{
	public:
		std::string_view source_file_path;
		std::string_view output_directory;
		TargetGpuApi	 target_gpu_api;
		Version			 shader_model;

		InputArgs(std::span<std::string_view> args);

	private:
		static constexpr std::string_view TARGET_GPU_API_PARAM = "--api=";
		static constexpr std::string_view OUTPUT_DIR_PARAM	   = "--out=";
		static constexpr std::string_view SHADER_MODEL_PARAM   = "--sm=";

		void parse_source_file_path(std::string_view first_arg);
		void parse_output_directory(std::span<std::string_view> args);
		void parse_target_gpu_api(std::span<std::string_view> args);
		void parse_shader_model(std::span<std::string_view> args);
	};
}
