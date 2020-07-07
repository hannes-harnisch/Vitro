#include "_pch.h"
#include "FragmentShader.h"

#include "Vitro/API/DirectX/RHI.h"
#include "Vitro/Utility/File.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler")

namespace Vitro::DirectX
{
	FragmentShader::FragmentShader(const File& file)
	{
		Bytecode = file.LoadBinary();
		auto res = RHI::Device->CreatePixelShader(Bytecode.Raw(), Bytecode.Count(), nullptr, &ShaderPtr);
		AssertCritical(SUCCEEDED(res), "Could not create fragment shader.");
	}

	FragmentShader::FragmentShader(const std::string& sourceCode, std::string& errors)
	{
#if VTR_DEBUG
		UINT compileFlags = D3DCOMPILE_DEBUG;
#else
		UINT compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
		Scope<ID3DBlob> bytecode;
		Scope<ID3DBlob> compileErrors;
		// WARN: Currently doesn't support macros, includes and entry points not named "main".
		D3DCompile(sourceCode.c_str(), sourceCode.length(), nullptr, nullptr, nullptr, "main", "ps_5_0", compileFlags,
				   0, &bytecode, &compileErrors);

		if(compileErrors)
		{
			errors = std::string(compileErrors->GetBufferSize(), '\0');
			std::memcpy(&errors[0], compileErrors->GetBufferPointer(), errors.length());
		}
		else
		{
			Bytecode = HeapArray<char>(bytecode->GetBufferSize());
			std::memcpy(Bytecode.Raw(), bytecode->GetBufferPointer(), Bytecode.ByteSize());
			RHI::Device->CreatePixelShader(Bytecode.Raw(), Bytecode.Count(), nullptr, &ShaderPtr);
		}
	}

	void FragmentShader::Bind() const
	{
		RHI::Context->PSSetShader(ShaderPtr, nullptr, 0);
	}
}
