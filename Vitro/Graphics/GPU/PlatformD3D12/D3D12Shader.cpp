module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <fstream>
export module vt.Graphics.D3D12.Shader;

import vt.Core.Array;

namespace vt::d3d12
{
	export class D3D12Shader
	{
	public:
		D3D12Shader(std::ifstream file) : bytecode(query_bytecode_size(file))
		{
			file.read(bytecode.data(), bytecode.size());
		}

		D3D12_SHADER_BYTECODE get_bytecode() const
		{
			return {
				.pShaderBytecode = bytecode.data(),
				.BytecodeLength	 = bytecode.size(),
			};
		}

	private:
		Array<char> bytecode;

		static size_t query_bytecode_size(std::ifstream& file)
		{
			file.seekg(0, std::ios::end);
			size_t size = file.tellg();
			file.seekg(0);
			return size;
		}
	};
}
