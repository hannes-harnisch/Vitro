module;
#include <concepts>
#include <concurrentqueue/concurrentqueue.h>
#include <condition_variable>
#include <cstdio>
#include <filesystem>
#include <mutex>
#include <ranges>
#include <source_location>
#include <string_view>
#include <thread>
module vt.Trace.Log;

import vt.Core.ConcurrentQueue;
import vt.Core.Enum;
import vt.Core.Singleton;
import vt.Trace.LogLevel;

namespace stdc = std::chrono;

namespace vt
{
	std::string make_timestamp(stdc::system_clock::duration now)
	{
		int64_t secs = stdc::duration_cast<stdc::seconds>(now).count();
		std::tm calendar_time;
		localtime_s(&calendar_time, &secs);

		std::string timestamp(sizeof "00:00:00", '\0');
		std::strftime(timestamp.data(), timestamp.capacity(), "%T", &calendar_time);
		return timestamp;
	}

	void Logger::disable_channel(LogChannel channel)
	{
		get().disabled_channels.set(channel, true);
	}

	void Logger::enable_channel(LogChannel channel)
	{
		get().disabled_channels.set(channel, false);
	}

	void Logger::disable_level(LogLevel level)
	{
		get().disabled_levels.set(level, true);
	}

	void Logger::enable_level(LogLevel level)
	{
		get().disabled_levels.set(level, false);
	}

	Logger::Logger() : con_token(queue), log_worker(&Logger::run_log_processing, this)
	{}

	Logger::~Logger()
	{
		is_accepting_logs = false;
		condition.notify_one();
	}

	void Logger::enqueue(LogLevel level, LogChannel channel, std::string message)
	{
		thread_local ProducerToken const pro_token(queue);

		auto now = stdc::system_clock::now().time_since_epoch();
		queue.enqueue(pro_token, {level, channel, now, std::move(message)});
		condition.notify_one();
	}

	bool Logger::is_channel_disabled(LogChannel channel) const
	{
		return disabled_channels[channel];
	}

	bool Logger::is_level_disabled(LogLevel level) const
	{
		return disabled_levels[level];
	}

	void Logger::run_log_processing()
	{
		while(is_accepting_logs)
			flush_queue();

		// Reset color sequence back to black background + white text.
		std::printf("\x1b[97;40m");
	}

	void Logger::flush_queue()
	{
		std::unique_lock lock(mutex);
		condition.wait(lock);
		lock.unlock();

		constexpr unsigned MAX_ENTRIES_AT_ONCE = 100;
		while(queue.size_approx())
			queue.consume<MAX_ENTRIES_AT_ONCE>(con_token, [&](Entry const& entry) {
				write_log(entry);
			});
	}

	void Logger::write_log(Entry const& entry)
	{
		auto level	 = enum_name(entry.level);
		auto channel = enum_name(entry.channel);

		auto	timestamp = make_timestamp(entry.time);
		int64_t millisecs = stdc::duration_cast<stdc::milliseconds>(entry.time).count() % 1000;

		auto esc_code_params = map_log_level_to_escape_code_parameters(entry.level);
		std::printf("\x1b[%sm\n[%s.%03lli|%s|%s] %s", esc_code_params, timestamp.data(), millisecs, level.data(),
					channel.data(), entry.message.data());
	}
}
