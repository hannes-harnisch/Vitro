module;
#include <string_view>
export module vttool.HlslBuilder.InvokeFxc;

namespace vt::tool
{
	export void invoke_fxc(std::string_view input_path, std::string_view output_path, std::string_view type);
}
