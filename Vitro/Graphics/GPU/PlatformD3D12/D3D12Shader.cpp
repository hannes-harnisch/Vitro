module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <fstream>
#include <vector>
export module vt.Graphics.D3D12.Shader;

namespace vt::d3d12
{
	export class D3D12Shader
	{
	public:
		D3D12Shader(std::ifstream file) : bytecode(std::istreambuf_iterator<char>(file), {})
		{}

		D3D12_SHADER_BYTECODE get_bytecode() const
		{
			return {
				.pShaderBytecode = bytecode.data(),
				.BytecodeLength	 = bytecode.size(),
			};
		}

	private:
		std::vector<char> bytecode;
	};
}
