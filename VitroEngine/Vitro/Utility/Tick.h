#pragma once

#include "_pch.h"

namespace Vitro
{
	struct Tick
	{
	public:
		static uint64_t MeasureTime()
		{
			using namespace std::chrono;
			auto now = steady_clock::now().time_since_epoch();
			return duration_cast<microseconds>(now).count();
		}

		Tick() = default;

		Tick(uint64_t previousTime, uint64_t currentTime)
		{
			Secs = (currentTime - previousTime) / 1000000.f;
		}

		float Seconds() const
		{
			return Secs;
		}
		float Milliseconds() const
		{
			return Secs * 1000;
		}

		bool operator==(Tick other) const
		{
			return Secs == other.Secs;
		}
		bool operator!=(Tick other) const
		{
			return Secs != other.Secs;
		}
		bool operator<(Tick other) const
		{
			return Secs < other.Secs;
		}
		bool operator>(Tick other) const
		{
			return Secs > other.Secs;
		}
		bool operator<=(Tick other) const
		{
			return Secs <= other.Secs;
		}
		bool operator>=(Tick other) const
		{
			return Secs >= other.Secs;
		}

		std::string ToString() const
		{
			float ms	  = Milliseconds();
			size_t length = _scprintf("%4.3f", ms);
			std::string str(length, '\0');
			sprintf_s(&str[0], length + 1, "%4.3f", ms);
			return str + " ms";
		}

		operator float() const
		{
			return Secs;
		}

	private:
		float Secs = 0.0f;
	};
}
