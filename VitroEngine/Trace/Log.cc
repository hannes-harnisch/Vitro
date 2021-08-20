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
export module Vitro.Trace.Log;

import Vitro.Core.ConcurrentQueue;
import Vitro.Core.Enum;
import Vitro.Core.Singleton;
import Vitro.Trace.LogLevel;

namespace stdc = std::chrono;

namespace vt
{
	export enum class LogChannel : unsigned char {
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
	concept IndirectlyConvertibleToString = requires(T t)
	{
		t->toString();
	};

	template<typename T>
	concept HasStdToStringOverload = !std::same_as<bool, T> && !std::is_enum_v<T> && !IndirectlyConvertibleToString<T> &&
									 requires(T t)
	{
		std::to_string(t);
	};

	export class Logger : public Singleton<Logger>
	{
		friend class Log;

	public:
		static void disableChannel(LogChannel channel)
		{
			get().atomicallySetDisabledChannels(channel, true);
		}

		static void enableChannel(LogChannel channel)
		{
			get().atomicallySetDisabledChannels(channel, false);
		}

		static void disableLevel(LogLevel level)
		{
			get().atomicallySetDisabledLevels(level, true);
		}

		static void enableLevel(LogLevel level)
		{
			get().atomicallySetDisabledLevels(level, false);
		}

		Logger() : conToken(queue), logWorker(&Logger::runLogProcessing, this)
		{}

		~Logger()
		{
			isAcceptingLogs = false;
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
		ConsumerToken						  conToken;
		std::mutex							  mutex;
		std::condition_variable				  condition;
		std::atomic<EnumBitArray<LogChannel>> disabledChannels;
		std::atomic<EnumBitArray<LogLevel>>	  disabledLevels;
		std::atomic_bool					  isAcceptingLogs = true;
		std::jthread						  logWorker;

		static std::string_view prepareArgument(bool arg)
		{
			return arg ? "true" : "false";
		}

		static std::string prepareArgument(HasStdToStringOverload auto arg)
		{
			return std::to_string(arg);
		}

		template<typename TEnum> static std::string_view prepareArgument(TEnum arg) requires std::is_enum_v<TEnum>
		{
			return enum_name(arg);
		}

		template<typename T>
		static std::string_view prepareArgument(T&& arg) requires(std::same_as<T, std::string> ||
																  std::same_as<T, std::string_view> ||
																  std::convertible_to<T, char const*>)
		{
			return arg;
		}

		static std::string prepareArgument(IndirectlyConvertibleToString auto&& arg)
		{
			return arg->toString();
		}

		static std::string prepareArgument(auto&& arg)
		{
			return arg.toString();
		}

		template<typename... Ts> static std::string makeMessage(Ts&&... ts)
		{
			// TODO: switch to stringstream after linker fix
			std::string str;
			(((str += ' ') += prepareArgument(std::forward<Ts>(ts))), ...);
			return str;
		}

		static std::string makeTimestamp(stdc::system_clock::duration now)
		{
			int64_t secs = stdc::duration_cast<stdc::seconds>(now).count();
			std::tm calendarTime;
			localtime_s(&calendarTime, &secs);

			std::string timestamp(sizeof "00:00:00", '\0');
			std::strftime(timestamp.data(), timestamp.capacity(), "%T", &calendarTime);
			return timestamp;
		}

		template<typename... Ts> void submit(LogLevel level, LogChannel channel, Ts&&... ts)
		{
			if(!isLevelDisabled(level) && !isChannelDisabled(channel))
				enqueue(level, channel, makeMessage(std::forward<Ts>(ts)...));
		}

		void enqueue(LogLevel level, LogChannel channel, std::string message)
		{
			thread_local ProducerToken const proToken(queue);

			auto now = stdc::system_clock::now().time_since_epoch();
			queue.enqueue(proToken, {level, channel, now, std::move(message)});
			condition.notify_one();
		}

		void atomicallySetDisabledChannels(LogChannel channel, bool value)
		{
			auto channels = disabledChannels.load();
			channels.set(channel, value);
			disabledChannels.store(channels);
		}

		void atomicallySetDisabledLevels(LogLevel level, bool value)
		{
			auto levels = disabledLevels.load();
			levels.set(level, value);
			disabledLevels.store(levels);
		}

		bool isChannelDisabled(LogChannel channel) const
		{
			return disabledChannels.load().test(channel);
		}

		bool isLevelDisabled(LogLevel level) const
		{
			return disabledLevels.load().test(level);
		}

		void runLogProcessing()
		{
			while(isAcceptingLogs)
				flushQueue();
		}

		void flushQueue()
		{
			std::unique_lock lock(mutex);
			condition.wait(lock);
			lock.unlock();

			while(queue.size_approx())
			{
				constexpr unsigned MaxEntriesDequeued = 100;

				Entry  entries[MaxEntriesDequeued];
				size_t count = queue.try_dequeue_bulk(conToken, entries, MaxEntriesDequeued);
				for(auto const& entry : std::views::take(entries, count))
					writeLog(entry);
			}
		}

		void writeLog(Entry const& entry)
		{
			auto level	 = enum_name(entry.level);
			auto channel = enum_name(entry.channel);

			auto	timestamp = makeTimestamp(entry.time);
			int64_t millisecs = stdc::duration_cast<stdc::milliseconds>(entry.time).count() % 1000;

			auto escCodeParams = mapLogLevelToEscapeCodeParameters(entry.level);
			std::printf("\x1b[%sm[ %s.%03lli | %s | %s ]%s\n", escCodeParams, timestamp.data(), millisecs, level.data(),
						channel.data(), entry.message.data());
		}
	};

	consteval LogChannel extractChannelFromPath(std::source_location src)
	{
		std::string_view path = src.file_name();

		size_t dirBegin		= path.rfind(VT_ENGINE_NAME) + sizeof VT_ENGINE_NAME;
		auto   pathAfterDir = path.substr(dirBegin);
		size_t dirEnd		= pathAfterDir.find(std::filesystem::path::preferred_separator);
		auto   dir			= pathAfterDir.substr(0, dirEnd);
		auto   channel		= enum_cast<LogChannel>(dir);

		auto   restPath		 = pathAfterDir.substr(dirEnd + 1);
		size_t vendorEnd	 = restPath.find(std::filesystem::path::preferred_separator);
		auto   vendorDir	 = restPath.substr(0, vendorEnd);
		auto   vendorChannel = enum_cast<LogChannel>(vendorDir);

		return channel.value_or(vendorChannel.value_or(LogChannel::Client));
	}

	export class Log
	{
	public:
		// TODO false compiler error because of consteval
		Log(/*LogChannel channel = extractChannelFromPath(std::source_location::current())*/) :
			channel(extractChannelFromPath(std::source_location::current()))
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
