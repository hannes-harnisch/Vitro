#pragma once

#include <D3D12MemoryAllocator/src/D3D12MemAlloc.h>
#include <d3d12.h>
#include <dxgi1_5.h>

#define VT_COM_OUT(PTR) __uuidof(decltype(*PTR)), std::out_ptr(PTR)
#include <memory>
