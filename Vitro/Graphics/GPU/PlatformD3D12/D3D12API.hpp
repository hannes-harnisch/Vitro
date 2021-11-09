#pragma once

#include <D3D12MemoryAllocator/D3D12MemoryAllocator/include/D3D12MemAlloc.h>
#include <d3d12.h>
#include <dxgi1_5.h>

// Replacement for IID_PPV_ARGS, as that macro has problems working with std::out_ptr.
#define VT_COM_OUT(PTR) __uuidof(decltype(*PTR)), std::out_ptr(PTR)

#include <memory>
