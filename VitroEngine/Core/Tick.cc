module;
#include <chrono>
#include <cstdint>
#include <format>
export module Vitro.Core.Tick;

export struct Tick
{
public:
	static uint64_t measureTime()
	{
		using namespace std::chrono;
		const auto now = steady_clock::now().time_since_epoch();
		return duration_cast<microseconds>(now).count();
	}

	Tick() = default;

	void update(uint64_t& previousTime)
	{
		const uint64_t current = Tick::measureTime();
		secs				   = (current - previousTime) / 1000000.0f;
		previousTime		   = current;
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

	std::string toString() const
	{
		return std::format("{} ms", milliseconds());
	}

	operator float() const
	{
		return secs;
	}

private:
	float secs {};
};
