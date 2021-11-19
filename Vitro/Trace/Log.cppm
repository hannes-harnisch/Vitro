module;
#include <concepts>
#include <concurrentqueue/concurrentqueue.h>
#include <condition_variable>
#include <cstdio>
#include <filesystem>
#include <mutex>
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
		static void disable_channel(LogChannel channel);
		static void enable_channel(LogChannel channel);
		static void disable_level(LogLevel level);
		static void enable_level(LogLevel level);

		Logger();
		~Logger();

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

		template<typename... Ts> void submit(LogLevel level, LogChannel channel, Ts&&... ts)
		{
			if(!is_level_disabled(level) && !is_channel_disabled(channel))
				enqueue(level, channel, make_message(std::forward<Ts>(ts)...));
		}

		void enqueue(LogLevel level, LogChannel channel, std::string message);
		bool is_channel_disabled(LogChannel channel) const;
		bool is_level_disabled(LogLevel level) const;
		void run_log_processing();
		void flush_queue();
		void write_log(Entry const& entry);
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
