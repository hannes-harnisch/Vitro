module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <ranges>
#include <span>
export module Vitro.D3D12.Device;

import Vitro.D3D12.Queue;
import Vitro.D3D12.Utils;
import Vitro.Graphics.Adapter;
import Vitro.Graphics.DeviceBase;
import Vitro.Graphics.Driver;

namespace vt::d3d12
{
	export class Device final : public DeviceBase
	{
	public:
		Device(vt::Driver const& driver, vt::Adapter adapter) :
			device(makeDevice(driver, adapter)),
			renderQueue(device.get(), D3D12_COMMAND_LIST_TYPE_DIRECT),
			computeQueue(device.get(), D3D12_COMMAND_LIST_TYPE_COMPUTE),
			copyQueue(device.get(), D3D12_COMMAND_LIST_TYPE_COPY)
		{}

		Receipt submitRenderCommands(std::span<void*> commandLists) override
		{
#if VT_DEBUG
			validateCommandLists<D3D12_COMMAND_LIST_TYPE_DIRECT>(commandLists);
#endif
			return {renderQueue.submit(commandLists)};
		}

		Receipt submitComputeCommands(std::span<void*> commandLists) override
		{
#if VT_DEBUG
			validateCommandLists<D3D12_COMMAND_LIST_TYPE_COMPUTE>(commandLists);
#endif
			return {computeQueue.submit(commandLists)};
		}

		Receipt submitCopyCommands(std::span<void*> commandLists) override
		{
#if VT_DEBUG
			validateCommandLists<D3D12_COMMAND_LIST_TYPE_COPY>(commandLists);
#endif
			return {copyQueue.submit(commandLists)};
		}

		void waitForRenderWorkload(Receipt receipt) override
		{
			renderQueue.waitForFenceValue(receipt.value);
		}

		void waitForComputeWorkload(Receipt receipt) override
		{
			computeQueue.waitForFenceValue(receipt.value);
		}

		void waitForCopyWorkload(Receipt receipt) override
		{
			copyQueue.waitForFenceValue(receipt.value);
		}

		void flushRenderQueue() override
		{
			renderQueue.waitForIdle();
		}

		void flushComputeQueue() override
		{
			computeQueue.waitForIdle();
		}

		void flushCopyQueue() override
		{
			copyQueue.waitForIdle();
		}

		void waitForIdle() override
		{
			renderQueue.waitForIdle();
			computeQueue.waitForIdle();
			copyQueue.waitForIdle();
		}

		ID3D12Device1* get() const
		{
			return device.get();
		}

		ID3D12CommandQueue* renderQueueHandle() const
		{
			return renderQueue.get();
		}

	private:
		ComUnique<ID3D12Device1> device;
		Queue					 renderQueue;
		Queue					 computeQueue;
		Queue					 copyQueue;

		static ID3D12Device1* makeDevice(vt::Driver const& driver, vt::Adapter& adapter)
		{
			auto d3d12CreateDevice = driver.d3d12.getDeviceCreationFunction();

			ID3D12Device1* device;

			auto result = d3d12CreateDevice(adapter.d3d12.get(), Driver::FeatureLevel, IID_PPV_ARGS(&device));
			vtEnsureResult(result, "Failed to create D3D12 device.");

			return device;
		}

		template<D3D12_COMMAND_LIST_TYPE Type> static void validateCommandLists(std::span<void*> commandLists)
		{
			for(auto list : commandLists)
			{
				bool isRightType = static_cast<ID3D12CommandList*>(list)->GetType() == Type;
				vtAssert(isRightType, "The type of this command list does not match the type of queue it was submitted to.");
			}
		}
	};
}
