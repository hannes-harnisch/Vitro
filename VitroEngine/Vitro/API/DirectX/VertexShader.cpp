#include "_pch.h"
#include "VertexShader.h"

#include "Vitro/API/DirectX/RHI.h"
#include "Vitro/Utility/File.h"
#include "Vitro/Utility/StackArray.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler")

namespace Vitro::DirectX
{
	VertexShader::VertexShader(const File& file)
	{
		Bytecode = file.LoadBinary();
		auto res = RHI::Device->CreateVertexShader(Bytecode.Raw(), Bytecode.Count(), nullptr, &ShaderPtr);
		AssertCritical(SUCCEEDED(res), "Could not create vertex shader.");
	}

	VertexShader::VertexShader(const std::string& sourceCode, std::string& errors)
	{
#if VTR_DEBUG
		UINT compileFlags = D3DCOMPILE_DEBUG;
#else
		UINT compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
		Scope<ID3DBlob> bytecode;
		Scope<ID3DBlob> compileErrors;
		// WARN: Currently doesn't support macros, includes and entry points not named "main".
		D3DCompile(sourceCode.c_str(), sourceCode.length(), nullptr, nullptr, nullptr, "main", "vs_5_0", compileFlags,
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
			RHI::Device->CreateVertexShader(Bytecode.Raw(), Bytecode.Count(), nullptr, &ShaderPtr);
		}
	}

	void VertexShader::Bind() const
	{
		RHI::Context->VSSetShader(ShaderPtr, nullptr, 0);
	}

	void VertexShader::SetVertexLayout(const VertexLayout& vl)
	{
		StackArray<D3D11_INPUT_ELEMENT_DESC> ieds(vl.Count());

		auto src = vl.begin();
		for(auto dst = ieds.begin(); dst != ieds.end(); ++src, ++dst)
		{
			dst->SemanticName		  = src->Name.c_str();
			dst->SemanticIndex		  = src->Index;
			dst->Format				  = static_cast<DXGI_FORMAT>(src->Type);
			dst->InputSlot			  = 0;
			dst->AlignedByteOffset	  = src->Offset;
			dst->InputSlotClass		  = D3D11_INPUT_PER_VERTEX_DATA;
			dst->InstanceDataStepRate = 0;
		}

		Scope<ID3D11InputLayout> il;
		auto res = RHI::Device->CreateInputLayout(ieds.Raw(), static_cast<UINT>(ieds.Count()), Bytecode.Raw(),
												  Bytecode.Count(), &il);
		AssertCritical(SUCCEEDED(res), "Could not create vertex layout.");
		RHI::Context->IASetInputLayout(il);
	}
}
