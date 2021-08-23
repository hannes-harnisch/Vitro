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
		PFN_D3D12_CREATE_VERSIONED_ROOT_SIGNATURE_DESERIALIZER const d3d12CreateVersionedRootSignatureDeserializer;
		PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE const			 d3d12SerializeVersionedRootSignature;

		Device(vt::Driver const& driver, vt::Adapter adapter) :
			d3d12CreateVersionedRootSignatureDeserializer(
				driver.d3d12.loadD3D12Function<PFN_D3D12_CREATE_VERSIONED_ROOT_SIGNATURE_DESERIALIZER>(
					"D3D12CreateVersionedRootSignatureDeserializer")),
			d3d12SerializeVersionedRootSignature(driver.d3d12.loadD3D12Function<PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE>(
				"D3D12SerializeVersionedRootSignature")),
			device(makeDevice(driver, std::move(adapter))),
			renderQueue(device.get(), D3D12_COMMAND_LIST_TYPE_DIRECT),
			computeQueue(device.get(), D3D12_COMMAND_LIST_TYPE_COMPUTE),
			copyQueue(device.get(), D3D12_COMMAND_LIST_TYPE_COPY)
		{}

		Receipt submitRenderCommands(std::span<vt::CommandListHandle> commandLists) override
		{
#if VT_DEBUG
			validateCommandLists(commandLists, D3D12_COMMAND_LIST_TYPE_DIRECT);
#endif
			return {renderQueue.submit(commandLists)};
		}

		Receipt submitComputeCommands(std::span<vt::CommandListHandle> commandLists) override
		{
#if VT_DEBUG
			validateCommandLists(commandLists, D3D12_COMMAND_LIST_TYPE_COMPUTE);
#endif
			return {computeQueue.submit(commandLists)};
		}

		Receipt submitCopyCommands(std::span<vt::CommandListHandle> commandLists) override
		{
#if VT_DEBUG
			validateCommandLists(commandLists, D3D12_COMMAND_LIST_TYPE_COPY);
#endif
			return {copyQueue.submit(commandLists)};
		}

		void waitForRenderWorkload(Receipt receipt) override
		{
			renderQueue.waitForFenceValue(receipt.hostWaitValue);
		}

		void waitForComputeWorkload(Receipt receipt) override
		{
			computeQueue.waitForFenceValue(receipt.hostWaitValue);
		}

		void waitForCopyWorkload(Receipt receipt) override
		{
			copyQueue.waitForFenceValue(receipt.hostWaitValue);
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

		ID3D12Device4* get() const
		{
			return device.get();
		}

		ID3D12CommandQueue* renderQueueHandle() const
		{
			return renderQueue.get();
		}

	private:
		ComUnique<ID3D12Device4> device;
		Queue					 renderQueue;
		Queue					 computeQueue;
		Queue					 copyQueue;

		static decltype(device) makeDevice(vt::Driver const& driver, vt::Adapter adapter)
		{
			ID3D12Device4* rawDevice;
			auto result = driver.d3d12.d3d12CreateDevice(adapter.d3d12.get(), Driver::FeatureLevel, IID_PPV_ARGS(&rawDevice));
			decltype(device) freshDevice(rawDevice);

			vtEnsureResult(result, "Failed to create D3D12 device.");
			return freshDevice;
		}

		static void validateCommandLists(std::span<vt::CommandListHandle> commandLists, D3D12_COMMAND_LIST_TYPE type)
		{
			for(auto list : commandLists)
			{
				bool isRightType = list.d3d12->GetType() == type;
				vtAssert(isRightType, "The type of this command list does not match the type of queue it was submitted to.");
			}
		}
	};
}
