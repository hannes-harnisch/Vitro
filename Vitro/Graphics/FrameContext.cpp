module;
#include <utility>
export module vt.Graphics.FrameContext;

import vt.Core.FixedList;

namespace vt
{
	export constexpr inline unsigned MAX_FRAMES_IN_FLIGHT = 3;

	// Implements a simple ring-buffer-like data structure that holds a globally defined number of instances of T and allows
	// switching between them.
	export template<typename T> class FrameContext
	{
	public:
		FrameContext(auto&... args)
		{
			for(int i = 0; i != MAX_FRAMES_IN_FLIGHT; ++i)
				frame_resources.emplace_back(args...);
		}

		T& operator*() noexcept
		{
			return frame_resources[index];
		}

		T const& operator*() const noexcept
		{
			return frame_resources[index];
		}

		T* operator->() noexcept
		{
			return &frame_resources[index];
		}

		T const* operator->() const noexcept
		{
			return &frame_resources[index];
		}

		void move_to_next_frame()
		{
			index = (index + 1) % frame_resources.size();
		}

		T& oldest() noexcept
		{
			return frame_resources[(index + frame_resources.size() - 1) % frame_resources.size()];
		}

		T const& oldest() const noexcept
		{
			return frame_resources[(index + frame_resources.size() - 1) % frame_resources.size()];
		}

		unsigned current_index() const noexcept
		{
			return index;
		}

	private:
		FixedList<T, MAX_FRAMES_IN_FLIGHT> frame_resources;
		unsigned						   index = 0;
	};
}
