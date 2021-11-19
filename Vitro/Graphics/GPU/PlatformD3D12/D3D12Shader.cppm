module;
#include "D3D12API.hpp"

#include <fstream>
export module vt.Graphics.D3D12.Shader;

import vt.Core.Array;

namespace vt::d3d12
{
	export class D3D12Shader
	{
	public:
		D3D12Shader(std::ifstream file);

		D3D12_SHADER_BYTECODE get_bytecode() const;

	private:
		Array<char> bytecode;
	};
}
