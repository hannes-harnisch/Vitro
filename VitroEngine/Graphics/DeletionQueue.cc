export module Vitro.Graphics.DeletionQueue;

import Vitro.Graphics.DeletionQueueBase;
import Vitro.Graphics.InterfaceVariant;

#if VT_DYNAMIC_GRAPHICS_API

import Vitro.D3D12.DeletionQueue;
import Vitro.Vulkan.DeletionQueue;

namespace vt
{
	export using DeletionQueue = InterfaceVariant<DeletionQueueBase, D3D12::DeletionQueue, Vulkan::DeletionQueue>;
}

#else

import Vitro.VT_GRAPHICS_API_MODULE.DeletionQueue;

namespace vt
{
	export using DeletionQueue = InterfaceVariant<DeletionQueueBase, VT_GRAPHICS_API_NAME::DeletionQueue>;
}

#endif
