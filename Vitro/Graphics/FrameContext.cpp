module;
#include <utility>
export module vt.Graphics.FrameContext;

import vt.Core.FixedList;

namespace vt
{
	export constexpr inline unsigned MAX_FRAMES_IN_FLIGHT = 3;

	export template<typename T> class FrameContext
	{
	public:
		template<typename... Ts> FrameContext(unsigned frames_in_flight, Ts&&... ts)
		{
			for(unsigned i = 0; i != frames_in_flight; ++i)
				frame_resources.emplace_back(std::forward<Ts>(ts)...);
		}

		T* operator->()
		{
			return &frame_resources[index];
		}

		T const* operator->() const
		{
			return &frame_resources[index];
		}

		unsigned current_index() const
		{
			return index;
		}

		void move_to_next_frame()
		{
			index = (index + 1) % frame_resources.size();
		}

	private:
		FixedList<T, MAX_FRAMES_IN_FLIGHT> frame_resources;
		unsigned						   index = 0;
	};
}
