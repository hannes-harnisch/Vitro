module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <fstream>
#include <vector>
export module vt.Graphics.Vulkan.Shader;

import vt.Graphics.Vulkan.Driver;

namespace vt::vulkan
{
	export class VulkanShader
	{
	public:
		VulkanShader(char const path[], DeviceApiTable const& api)
		{
			std::ifstream	  file(path, std::ios::binary);
			std::vector<char> bytecode(std::istreambuf_iterator<char>(file), {});

			VkShaderModuleCreateInfo const shader_info {
				.sType	  = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				.codeSize = bytecode.size(),
				.pCode	  = reinterpret_cast<uint32_t const*>(bytecode.data()),
			};
			VkShaderModule unowned_shader;

			auto result = api.vkCreateShaderModule(api.device, &shader_info, nullptr, &unowned_shader);
			shader.reset(unowned_shader, api);
			VT_ENSURE_RESULT(result, "Failed to create Vulkan shader module.");
		}

		VkShaderModule ptr() const
		{
			return shader.get();
		}

	private:
		UniqueVkShaderModule shader;
	};
}
