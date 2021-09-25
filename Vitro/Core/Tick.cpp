module;
#include <chrono>
#include <cstdint>
#include <format>
export module vt.Core.Tick;

namespace stdc = std::chrono;

namespace vt
{
	export struct Tick
	{
	public:
		static uint64_t measure_time()
		{
			auto now = stdc::steady_clock::now().time_since_epoch();
			return stdc::duration_cast<stdc::microseconds>(now).count();
		}

		Tick() = default;

		void update(uint64_t& previous_time)
		{
			uint64_t current = Tick::measure_time();
			secs			 = (current - previous_time) / 1000000.0f;
			previous_time	 = current;
		}

		float seconds() const
		{
			return secs;
		}

		float milliseconds() const
		{
			return secs * 1000;
		}

		auto operator<=>(Tick other) const
		{
			return secs <=> other.secs;
		}

		std::string to_string() const
		{
			return std::format("{:.3f} ms", milliseconds());
		}

		operator float() const
		{
			return secs;
		}

	private:
		float secs = 0;
	};
}
