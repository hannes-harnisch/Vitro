export module Vitro.Graphics.FrameContext;

namespace vt
{
	export class FrameContext
	{
	public:
		static constexpr unsigned MaxInFlight = 2;

		void changeIndex()
		{
			currentIndex = (currentIndex + 1) % MaxInFlight;
		}

	private:
		unsigned currentIndex = 0;
	};
}
