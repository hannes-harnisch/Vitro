#pragma once

#include "Vitro/API/DirectX/Scope.h"
#include "Vitro/Graphics/Texture.h"

#include <d3d11_4.h>

namespace Vitro::DirectX
{
	class Texture2D final : public Vitro::Texture2D
	{
	public:
		Texture2D(const std::string& filePath);

		void Bind() const override;

	private:
		Scope<ID3D11Texture2D> Texture;
		Scope<ID3D11ShaderResourceView> Resource;
		Scope<ID3D11SamplerState> Sampler;
	};
}
