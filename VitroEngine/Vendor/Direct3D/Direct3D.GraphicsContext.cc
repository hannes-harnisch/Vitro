module;
#include "Direct3D.API.hh"
#include "Trace/Assert.hh"

#include <array>
export module Vitro.Direct3D.GraphicsContext;

import Vitro.Direct3D.Unique;

class GraphicsSystem;

namespace Direct3D
{
	export class GraphicsContext final
	{
		friend ::GraphicsSystem;

	private:
		constexpr static D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;

#if VE_DEBUG
		Unique<ID3D12Debug> debug;
#endif
		Unique<IDXGIAdapter1> adapter;
		Unique<ID3D12Device2> device;
		Unique<ID3D12CommandQueue> graphicsQueue;
		Unique<ID3D12CommandQueue> copyQueue;

		GraphicsContext()
		{
			createDebugInterface();
			createAdapter();
			createDevice();
			createQueues();
		}

		void createDebugInterface()
		{
#if VE_DEBUG
			veEnsureResult(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)), "Cannot get debug interface.");
			debug->EnableDebugLayer();
#endif
		}

		void createAdapter()
		{
			Unique<IDXGIFactory2> factory;
			UINT factoryFlags {};
#if VE_DEBUG
			factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
			veEnsureResult(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory)), "Cannot get DXGI factory.");

			SIZE_T maxDedicatedVideoMemory {};
			UINT index {};
			while(factory->EnumAdapters1(index, &adapter) != DXGI_ERROR_NOT_FOUND)
			{
				DXGI_ADAPTER_DESC1 adapterInfo;
				adapter->GetDesc1(&adapterInfo);

				const bool isNotSoftwareAdapter = (adapterInfo.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0;
				const bool canCreateDevice =
					SUCCEEDED(D3D12CreateDevice(adapter, FeatureLevel, __uuidof(ID3D12Device), nullptr));
				const bool vramIsSufficient = adapterInfo.DedicatedVideoMemory > maxDedicatedVideoMemory;
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
			veEnsureResult(D3D12CreateDevice(adapter, FeatureLevel, IID_PPV_ARGS(&device)), "Cannot get D3D12 device.");

#if VE_DEBUG
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
			veEnsureResult(infoQueue->PushStorageFilter(&filter), "Cannot get D3D12 debug message filter.");
#endif
		}

		void createQueues()
		{
			D3D12_COMMAND_QUEUE_DESC desc {};
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			veEnsureResult(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&graphicsQueue)), "Cannot get D3D12 graphics queue.");

			desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
			veEnsureResult(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&copyQueue)), "Cannot get D3D12 copy queue.");
		}

		bool checkForTearingSupport()
		{
			BOOL hasTearing {};

			// Rather than create the DXGI 1.5 factory interface directly, we create the
			// DXGI 1.4 interface and query for the 1.5 interface. This is to enable the
			// graphics debugging tools which will not support the 1.5 factory interface
			// until a future update.
			Unique<IDXGIFactory4> factory4;
			if(SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
			{
				Unique<IDXGIFactory5> factory5;
				if(SUCCEEDED(factory4.queryFor(&factory5)))
					if(FAILED(
						   factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &hasTearing, sizeof hasTearing)))
						hasTearing = FALSE;
			}
			return hasTearing == TRUE;
		}
	};
}
