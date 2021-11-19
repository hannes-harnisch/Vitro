module;
#include "VitroCore/Macros.hpp"

#include <format>
#include <memory>
#include <stdexcept>
#include <string_view>

#if VT_GPU_API_D3D12
	#include <d3dcompiler.h>
#endif
module vttool.HlslBuilder.InvokeFxc;

#if VT_GPU_API_D3D12
import vt.Core.Windows.Utils;
#endif

namespace vt::tool
{
#if VT_GPU_API_D3D12

	void invoke_fxc(std::string_view input_path, std::string_view output_path, std::string_view type)
	{
		using namespace windows;
		auto wide_input_path = widen_string(input_path);

		D3D_SHADER_MACRO macros[] {
			{"VT_GPU_API_D3D12", "1"},
			{},
		};
		ComUnique<ID3DBlob> code_blob, error_blob;
		auto result = D3DCompileFromFile(wide_input_path.data(), macros, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", type.data(),
										 D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, std::out_ptr(code_blob), std::out_ptr(error_blob));
		if(FAILED(result))
		{
			auto error = static_cast<char const*>(error_blob->GetBufferPointer());
			auto msg   = std::format("Shader compilation failed:\n\n{}", error);
			throw std::invalid_argument(msg);
		}

		auto wide_output_path = widen_string(output_path);

		result = D3DWriteBlobToFile(code_blob.get(), wide_output_path.data(), true);
		VT_CHECK_RESULT(result, "Failed to write compiled shader output to file.");
	}

#else

	void invoke_fxc(std::string_view, std::string_view, std::string_view)
	{
		throw std::runtime_error("A shader cannot be compiled for D3D12 when HLSL Builder is not built under Windows.");
	}

#endif
}
