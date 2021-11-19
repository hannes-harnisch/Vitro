module;
#include "D3D12API.hpp"

#include <string>
export module vt.Graphics.D3D12.Driver;

import vt.Core.Version;
import vt.Core.Windows.Utils;
import vt.Graphics.D3D12.Device;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.AbstractDriver;

namespace vt::d3d12
{
	export class D3D12Driver final : public AbstractDriver
	{
	public:
		// The unused parameters are kept for compatibility with Vulkan, which takes an application name and versions.
		D3D12Driver(bool enable_debug_layer, std::string const&, Version, Version);

		SmallList<Adapter> enumerate_adapters() const override;
		Device			   make_device(Adapter const& adapter) const override;

	private:
		ComUnique<ID3D12Debug>	 debug;
		ComUnique<IDXGIFactory5> factory;
	};
}
