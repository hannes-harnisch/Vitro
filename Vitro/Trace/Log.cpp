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
export module vt.Trace.Log;

import vt.Core.ConcurrentQueue;
import vt.Core.Enum;
import vt.Core.Singleton;
import vt.Trace.LogLevel;

namespace stdc = std::chrono;

namespace vt
{
	export enum class LogChannel : uint8_t {
		App,
		Asset,
		Audio,
		Client,
		Core,
		Editor,
		Graphics,
		Math,
		Physics,
		Trace,
		Windows,
		D3D12,
		Vulkan,
	};

	template<typename T, typename... Ts>
	concept OneOf = std::disjunction_v<std::is_same<T, Ts>...>;

	template<typename T> auto prepare_argument(T& value)
	{
		return value.to_string();
	}

	template<OneOf<int, long, long long, unsigned, unsigned long, unsigned long long, float, double, long double> T>
	auto prepare_argument(T value)
	{
		return std::to_string(value);
	}

	template<typename T>
	requires std::is_enum_v<T> std::string_view prepare_argument(T value)
	{
		return enum_name(value);
	}

	template<std::convertible_to<std::string_view> T> decltype(auto) prepare_argument(T& value)
	{
		return value;
	}

	char const* prepare_argument(char const* value)
	{
		return value;
	}

	std::string_view prepare_argument(std::string_view value)
	{
		return value;
	}

	char prepare_argument(char value)
	{
		return value;
	}

	char const* prepare_argument(bool value)
	{
		return value ? "true" : "false";
	}

	export class Logger : public Singleton<Logger>
	{
		friend class Log;

	public:
		static void disable_channel(LogChannel channel)
		{
			get().disabled_channels.set(channel, true);
		}

		static void enable_channel(LogChannel channel)
		{
			get().disabled_channels.set(channel, false);
		}

		static void disable_level(LogLevel level)
		{
			get().disabled_levels.set(level, true);
		}

		static void enable_level(LogLevel level)
		{
			get().disabled_levels.set(level, false);
		}

		Logger() : con_token(queue), log_worker(&Logger::run_log_processing, this)
		{}

		~Logger()
		{
			is_accepting_logs = false;
			condition.notify_one();
		}

	private:
		struct Entry
		{
			LogLevel					 level;
			LogChannel					 channel;
			stdc::system_clock::duration time;
			std::string					 message;
		};
		ConcurrentQueue<Entry>		   queue;
		ConsumerToken				   con_token;
		std::mutex					   mutex;
		std::condition_variable		   condition;
		AtomicEnumBitArray<LogChannel> disabled_channels;
		AtomicEnumBitArray<LogLevel>   disabled_levels;
		std::atomic_bool			   is_accepting_logs = true;
		std::jthread				   log_worker;

		template<typename... Ts> static std::string make_message(Ts&&... ts)
		{
			// TODO: wait for linker fix, then stringstream
			std::string str;
			((str += prepare_argument(std::forward<Ts>(ts))), ...);
			return str;
		}

		static std::string make_timestamp(stdc::system_clock::duration now)
		{
			int64_t secs = stdc::duration_cast<stdc::seconds>(now).count();
			std::tm calendar_time;
			localtime_s(&calendar_time, &secs);

			std::string timestamp(sizeof "00:00:00", '\0');
			std::strftime(timestamp.data(), timestamp.capacity(), "%T", &calendar_time);
			return timestamp;
		}

		template<typename... Ts> void submit(LogLevel level, LogChannel channel, Ts&&... ts)
		{
			if(!is_level_disabled(level) && !is_channel_disabled(channel))
				enqueue(level, channel, make_message(std::forward<Ts>(ts)...));
		}

		void enqueue(LogLevel level, LogChannel channel, std::string message)
		{
			thread_local ProducerToken const pro_token(queue);

			auto now = stdc::system_clock::now().time_since_epoch();
			queue.enqueue(pro_token, {level, channel, now, std::move(message)});
			condition.notify_one();
		}

		bool is_channel_disabled(LogChannel channel) const
		{
			return disabled_channels[channel];
		}

		bool is_level_disabled(LogLevel level) const
		{
			return disabled_levels[level];
		}

		void run_log_processing()
		{
			while(is_accepting_logs)
				flush_queue();

			// Reset color sequence back to black background + white text.
			std::printf("\x1b[97;40m");
		}

		void flush_queue()
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

		void write_log(Entry const& entry)
		{
			auto level	 = enum_name(entry.level);
			auto channel = enum_name(entry.channel);

			auto	timestamp = make_timestamp(entry.time);
			int64_t millisecs = stdc::duration_cast<stdc::milliseconds>(entry.time).count() % 1000;

			auto esc_code_params = map_log_level_to_escape_code_parameters(entry.level);
			std::printf("\x1b[%sm\n[%s.%03lli|%s|%s] %s", esc_code_params, timestamp.data(), millisecs, level.data(),
						channel.data(), entry.message.data());
		}
	};

	consteval LogChannel extract_channel_from_path(std::source_location src)
	{
		std::string_view path = src.file_name();

		size_t dir_begin	  = path.rfind(VT_ENGINE_NAME) + sizeof VT_ENGINE_NAME;
		auto   path_after_dir = path.substr(dir_begin);
		size_t dir_end		  = path_after_dir.find(std::filesystem::path::preferred_separator);
		auto   dir			  = path_after_dir.substr(0, dir_end);
		auto   channel		  = enum_cast<LogChannel>(dir);

		auto   rest_path	  = path_after_dir.substr(dir_end + 1);
		size_t vendor_end	  = rest_path.find(std::filesystem::path::preferred_separator);
		auto   vendor_dir	  = rest_path.substr(0, vendor_end);
		auto   vendor_channel = enum_cast<LogChannel>(vendor_dir);

		return channel.value_or(vendor_channel.value_or(LogChannel::Client));
	}

	export class Log
	{
	public:
		// TODO: wait for compiler fix, then uncomment to actually get the right source file category.
		Log(/*LogChannel channel = extract_channel_from_path(std::source_location::current())*/) :
			channel(extract_channel_from_path(std::source_location::current()))
		{}

		template<typename... Ts> void verbose(Ts&&... ts) const
		{
			log(LogLevel::Verbose, std::forward<Ts>(ts)...);
		}

		template<typename... Ts> void debug(Ts&&... ts) const
		{
			log(LogLevel::Debug, std::forward<Ts>(ts)...);
		}

		template<typename... Ts> void info(Ts&&... ts) const
		{
			log(LogLevel::Info, std::forward<Ts>(ts)...);
		}

		template<typename... Ts> void warn(Ts&&... ts) const
		{
			log(LogLevel::Warning, std::forward<Ts>(ts)...);
		}

		template<typename... Ts> void error(Ts&&... ts) const
		{
			log(LogLevel::Error, std::forward<Ts>(ts)...);
		}

		template<typename... Ts> void fatal(Ts&&... ts) const
		{
			log(LogLevel::Fatal, std::forward<Ts>(ts)...);
		}

	private:
		LogChannel channel;

		template<typename... Ts> void log(LogLevel level, Ts&&... ts) const
		{
			Logger::get().submit(level, channel, std::forward<Ts>(ts)...);
		}
	};
}
