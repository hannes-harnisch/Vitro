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

	template<typename T>
	concept HasToString = requires(T const& t)
	{
		{
			t.to_string()
			} -> std::convertible_to<std::string>;
	};

	template<typename T>
	concept HasStdToStringOverload = !std::same_as<bool, T> && !std::is_enum_v<T> && requires(T t)
	{
		std::to_string(t);
	};

	export class Logger : public Singleton<Logger>
	{
		friend class Log;

	public:
		static void disable_channel(LogChannel channel)
		{
			get().atomically_set_disabled_channels(channel, true);
		}

		static void enable_channel(LogChannel channel)
		{
			get().atomically_set_disabled_channels(channel, false);
		}

		static void disable_level(LogLevel level)
		{
			get().atomically_set_disabled_levels(level, true);
		}

		static void enable_level(LogLevel level)
		{
			get().atomically_set_disabled_levels(level, false);
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

		ConcurrentQueue<Entry>				  queue;
		ConsumerToken						  con_token;
		std::mutex							  mutex;
		std::condition_variable				  condition;
		std::atomic<EnumBitArray<LogChannel>> disabled_channels;
		std::atomic<EnumBitArray<LogLevel>>	  disabled_levels;
		std::atomic_bool					  is_accepting_logs = true;
		std::jthread						  log_worker;

		static std::string prepare_argument(HasToString auto&& arg)
		{
			return arg.to_string();
		}

		static char const* prepare_argument(bool arg)
		{
			return arg ? "true" : "false";
		}

		static std::string prepare_argument(HasStdToStringOverload auto arg)
		{
			return std::to_string(arg);
		}

		template<typename T> static std::string_view prepare_argument(T arg) requires std::is_enum_v<T>
		{
			return enum_name(arg);
		}

		template<typename T>
		static auto prepare_argument(T arg) requires(std::same_as<T, std::string> || std::same_as<T, std::string_view> ||
													 std::convertible_to<T, char const*>)
		{
			return arg;
		}

		template<typename... Ts> static std::string make_message(Ts&&... ts)
		{
			// TODO: switch to stringstream after linker fix
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

		void atomically_set_disabled_channels(LogChannel channel, bool value)
		{
			auto channels = disabled_channels.load();
			channels.set(channel, value);
			disabled_channels.store(channels);
		}

		void atomically_set_disabled_levels(LogLevel level, bool value)
		{
			auto levels = disabled_levels.load();
			levels.set(level, value);
			disabled_levels.store(levels);
		}

		bool is_channel_disabled(LogChannel channel) const
		{
			return disabled_channels.load().test(channel);
		}

		bool is_level_disabled(LogLevel level) const
		{
			return disabled_levels.load().test(level);
		}

		void run_log_processing()
		{
			while(is_accepting_logs)
				flush_queue();
		}

		void flush_queue()
		{
			std::unique_lock lock(mutex);
			condition.wait(lock);
			lock.unlock();

			while(queue.size_approx())
			{
				constexpr unsigned MAX_ENTRIES_DEQUEUED = 100;

				Entry  entries[MAX_ENTRIES_DEQUEUED];
				size_t count = queue.try_dequeue_bulk(con_token, entries, MAX_ENTRIES_DEQUEUED);
				for(auto const& entry : std::views::take(entries, count))
					write_log(entry);
			}
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
		// TODO false compiler error because of consteval
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
