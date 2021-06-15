module;
#include "Direct3D.API.hh"
#include "Trace/Assert.hh"

#include <array>
export module Vitro.Direct3D.DriverContext;

import Vitro.Direct3D.Unique;

class GraphicsSystem;

namespace Direct3D
{
	export class DriverContext final
	{
		friend ::GraphicsSystem;

	private:
		constexpr static D3D_FEATURE_LEVEL Level = D3D_FEATURE_LEVEL_11_0;

#if VT_DEBUG
		Unique<ID3D12Debug> debug;
#endif

		DriverContext()
		{
			createDebugInterface();
		}

		void createDebugInterface()
		{
#if VT_DEBUG
			vtEnsureResult(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)), "Cannot get debug interface.");
			debug->EnableDebugLayer();
#endif
		}

		void createAdapter()
		{
			Unique<IDXGIFactory2> factory;
			UINT factoryFlags {};
#if VT_DEBUG
			factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
			vtEnsureResult(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory)), "Cannot get DXGI factory.");

			SIZE_T maxDedicatedVideoMemory {};
			UINT index {};
			while(factory->EnumAdapters1(index, &adapter) != DXGI_ERROR_NOT_FOUND)
			{
				DXGI_ADAPTER_DESC1 adapterInfo;
				adapter->GetDesc1(&adapterInfo);

				bool const isNotSoftwareAdapter = (adapterInfo.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0;
				bool const canCreateDevice		= SUCCEEDED(D3D12CreateDevice(adapter, Level, __uuidof(ID3D12Device), nullptr));
				bool const vramIsSufficient		= adapterInfo.DedicatedVideoMemory > maxDedicatedVideoMemory;
				if(isNotSoftwareAdapter && canCreateDevice && vramIsSufficient)
				{
					maxDedicatedVideoMemory = adapterInfo.DedicatedVideoMemory;
					continue;
				}
				++index;
			}
		}

		void createDevice()
		{
			vtEnsureResult(D3D12CreateDevice(adapter, Level, IID_PPV_ARGS(&device)), "Cannot get D3D12 device.");

#if VT_DEBUG
			Unique<ID3D12InfoQueue> infoQueue;
			if(FAILED(device.queryFor(&infoQueue)))
				return;

			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

			std::array filteredSeverities {D3D12_MESSAGE_SEVERITY_INFO};
			std::array filteredIds {
				// I'm really not sure how to avoid this message.
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				// These two warning occurs when using capture frame while graphics debugging.
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
			};

			D3D12_INFO_QUEUE_FILTER filter {};
			filter.DenyList.NumSeverities = UINT(filteredSeverities.size());
			filter.DenyList.pSeverityList = filteredSeverities.data();
			filter.DenyList.NumIDs		  = UINT(filteredIds.size());
			filter.DenyList.pIDList		  = filteredIds.data();
			vtEnsureResult(infoQueue->PushStorageFilter(&filter), "Cannot get D3D12 debug message filter.");
#endif
		}

		void createQueues()
		{
			D3D12_COMMAND_QUEUE_DESC desc {};
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			vtEnsureResult(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&graphicsQueue)), "Cannot get D3D12 graphics queue.");

			desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
			vtEnsureResult(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&copyQueue)), "Cannot get D3D12 copy queue.");
		}
	};
}
