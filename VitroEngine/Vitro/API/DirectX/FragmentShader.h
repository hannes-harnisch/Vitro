#pragma once

#include "Vitro/API/DirectX/Scope.h"
#include "Vitro/Graphics/Shader.h"

#include <d3d11_4.h>

namespace Vitro::DirectX
{
	class FragmentShader final : public Vitro::FragmentShader
	{
	public:
		FragmentShader(const File& file);
		FragmentShader(const std::string& sourceCode, std::string& errors);

		void Bind() const override;

	private:
		Scope<ID3D11PixelShader> ShaderPtr;
	};
}
