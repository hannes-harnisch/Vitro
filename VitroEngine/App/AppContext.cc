module;
#include "Core/Macros.hh"

#include <unordered_map>
export module vt.App.AppContext;

import vt.App.EventListener;
import vt.App.ObjectEvent;
import vt.App.Window;
import vt.VT_SYSTEM_MODULE.AppContext;

namespace vt
{
	using AppContextBase = VT_SYSTEM_NAME::VT_PASTE(VT_SYSTEM_MODULE, AppContext);

	export class AppContext : public AppContextBase, public EventListener
	{
	public:
		AppContext()
		{
			register_event_handlers<&AppContext::on_window_object_construct, &AppContext::on_window_object_move_construct,
									&AppContext::on_window_object_destroy, &AppContext::on_window_object_move_assign>();
		}

		void poll_events() const
		{
			AppContextBase::poll_events();
		}

	private:
		void on_window_object_construct(ObjectConstructEvent<Window>& event)
		{
			auto& map = get_native_window_handle_map();
			map.try_emplace(event.object.native_handle(), &event.object);
		}

		void on_window_object_move_construct(ObjectMoveConstructEvent<Window>& event)
		{
			auto& map = get_native_window_handle_map();

			map[event.constructed.native_handle()] = &event.constructed;
		}

		void on_window_object_destroy(ObjectDestroyEvent<Window>& event)
		{
			auto& map = get_native_window_handle_map();
			map.erase(event.object.native_handle());
		}

		void on_window_object_move_assign(ObjectMoveAssignEvent<Window>& event)
		{
			auto& map = get_native_window_handle_map();

			std::erase_if(map, [&](auto const& pair) {
				return pair.second == &event.left;
			});
			map.try_emplace(event.left.native_handle(), &event.left);
		}
	};
}
