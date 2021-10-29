module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <fstream>
export module vt.Graphics.Vulkan.Shader;

import vt.Core.Array;
import vt.Graphics.Vulkan.Driver;

namespace vt::vulkan
{
	export class VulkanShader
	{
	public:
		VulkanShader(char const path[], DeviceApiTable const& api)
		{
			std::ifstream file(path, std::ios::binary);
			file.seekg(0, std::ios::end);
			Array<char> bytecode(file.tellg());
			file.seekg(0);
			file.read(bytecode.data(), bytecode.size());

			VkShaderModuleCreateInfo const shader_info {
				.sType	  = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				.codeSize = bytecode.size(),
				.pCode	  = reinterpret_cast<uint32_t const*>(bytecode.data()),
			};
			auto result = api.vkCreateShaderModule(api.device, &shader_info, nullptr, std::out_ptr(shader, api));
			VT_CHECK_RESULT(result, "Failed to create Vulkan shader module.");
		}

		VkShaderModule ptr() const
		{
			return shader.get();
		}

	private:
		UniqueVkShaderModule shader;
	};
}
