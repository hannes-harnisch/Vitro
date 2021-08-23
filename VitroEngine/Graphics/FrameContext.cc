module;
#include <utility>
export module Vitro.Graphics.FrameContext;

import Vitro.Core.FixedList;

namespace vt
{
	export constexpr unsigned MaxFramesInFlight = 3;

	export template<typename T> class FrameContext
	{
	public:
		template<typename... Ts> FrameContext(unsigned framesInFlight, Ts&&... ts)
		{
			for(unsigned i = 0; i != framesInFlight; ++i)
				frameResources.emplace_back(std::forward<Ts>(ts)...);
		}

		T* operator->()
		{
			return &frameResources[index];
		}

		T const* operator->() const
		{
			return &frameResources[index];
		}

		unsigned currentIndex() const
		{
			return index;
		}

		void moveToNextFrame()
		{
			index = (index + 1) % frameResources.size();
		}

	private:
		FixedList<T, MaxFramesInFlight> frameResources;
		unsigned						index = 0;
	};
}
