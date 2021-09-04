module;
#include <string_view>
export module vt.App.SharedLibraryBase;

namespace vt
{
	export class SharedLibraryBase
	{
	public:
		[[nodiscard]] virtual bool reload()		= 0;
		virtual void*			   get_handle() = 0;

	protected:
		virtual void* load_symbol_address(std::string_view symbol) const = 0;
	};
}
