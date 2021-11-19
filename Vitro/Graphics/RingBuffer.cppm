module;
#include <utility>
export module vt.Graphics.RingBuffer;

import vt.Core.FixedList;

namespace vt
{
	export constexpr inline unsigned MAX_FRAMES_IN_FLIGHT = 3;

	// Implements a simple ring-buffer-like data structure that holds a globally defined number of instances of T and allows
	// switching between them.
	export template<typename T> class RingBuffer
	{
	public:
		RingBuffer(auto&... args)
		{
			for(int i = 0; i != MAX_FRAMES_IN_FLIGHT; ++i)
				frame_resources.emplace_back(args...);
		}

		T& current() noexcept
		{
			return frame_resources[index];
		}

		T const& current() const noexcept
		{
			return frame_resources[index];
		}

		void move_to_next_frame()
		{
			index = (index + 1) % frame_resources.size();
		}

		T& get_previous() noexcept
		{
			size_t size = frame_resources.size();
			return frame_resources[(index + size - 1) % size];
		}

		T const& get_previous() const noexcept
		{
			size_t size = frame_resources.size();
			return frame_resources[(index + size - 1) % size];
		}

		unsigned get_current_index() const noexcept
		{
			return index;
		}

	private:
		FixedList<T, MAX_FRAMES_IN_FLIGHT> frame_resources;
		unsigned						   index = 0;
	};
}
