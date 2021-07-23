module;
#include "Core/Enum.hh"

#include <bitset>
#include <concepts>
#include <concurrentqueue/concurrentqueue.h> // TODO: Remove when type export doesn't crash compiler
#include <condition_variable>
#include <cstdio>
#include <filesystem>
#include <mutex>
#include <ranges>
#include <source_location>
#include <string_view>
export module Vitro.Trace.Log;

import Vitro.Core.ConcurrentQueue;
import Vitro.Core.Singleton;
import Vitro.Trace.LogLevel;

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

	template<typename T> concept IndirectlyConvertibleToString = requires(T t)
	{
		t->toString();
	};

	template<typename T>
	concept HasStdToStringOverload =
		!std::same_as<bool, T> && !std::is_enum_v<T> && !IndirectlyConvertibleToString<T> && requires(T t)
	{
		std::to_string(t);
	};

	export class Logger : public Singleton<Logger>
	{
		friend class TraceSystem;
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

	private:
		struct Entry
		{
			LogLevel level;
			LogChannel channel;
			std::chrono::system_clock::duration time;
			std::string message;
		};

		ConcurrentQueue<Entry> queue;
		ConsumerToken consumerToken;
		std::mutex mutex;
		std::condition_variable condition;
		std::atomic<std::bitset<sizeFromEnumMax<LogChannel>()>> disabledChannels;
		std::atomic<std::bitset<sizeFromEnumMax<LogLevel>()>> disabledLevels;
		std::atomic_bool isAcceptingLogs = true;

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
			return getEnumName(arg);
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
			std::string str;
			((str += prepareArgument(std::forward<Ts>(ts))), ...);
			return str;
		}

		static std::string makeTimestamp(std::chrono::system_clock::duration now)
		{
			using namespace std::chrono;
			int64_t secs = duration_cast<seconds>(now).count();
			std::tm calendarTime;
			localtime_s(&calendarTime, &secs);

			std::string timestamp(sizeof "00:00:00", '\0');
			std::strftime(timestamp.data(), timestamp.capacity(), "%T", &calendarTime);
			return timestamp;
		}

		Logger() : consumerToken(queue)
		{}

		template<typename... Ts> void submit(LogLevel level, LogChannel channel, Ts&&... ts)
		{
			if(!isLevelDisabled(level) && !isChannelDisabled(channel))
				enqueue(level, channel, makeMessage(std::forward<Ts>(ts)...));
		}

		void enqueue(LogLevel level, LogChannel channel, std::string message)
		{
			static thread_local ProducerToken const producerToken(queue);
			auto now = std::chrono::system_clock::now().time_since_epoch();
			queue.enqueue(producerToken, {level, channel, now, std::move(message)});
			condition.notify_one();
		}

		void atomicallySetDisabledChannels(LogChannel channel, bool value)
		{
			auto channels = disabledChannels.load();
			size_t index  = *getEnumIndex(channel);
			channels.set(index, value);
			disabledChannels.store(channels);
		}

		void atomicallySetDisabledLevels(LogLevel level, bool value)
		{
			auto levels	 = disabledLevels.load();
			size_t index = *getEnumIndex(level);
			levels.set(index, value);
			disabledLevels.store(levels);
		}

		bool isChannelDisabled(LogChannel channel) const
		{
			size_t channelIndex = *getEnumIndex(channel);
			return disabledChannels.load().test(channelIndex);
		}

		bool isLevelDisabled(LogLevel level) const
		{
			size_t levelIndex = *getEnumIndex(level);
			return disabledLevels.load().test(levelIndex);
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

			constexpr unsigned MaxEntriesDequeued = 100;
			Entry entries[MaxEntriesDequeued];
			size_t count = queue.try_dequeue_bulk(consumerToken, entries, MaxEntriesDequeued);
			for(auto const& entry : std::views::take(entries, count))
				writeLog(entry);
		}

		void writeLog(Entry const& entry)
		{
			auto level	 = getEnumName(entry.level);
			auto channel = getEnumName(entry.channel);

			using namespace std::chrono;
			auto timestamp	  = makeTimestamp(entry.time);
			int64_t millisecs = duration_cast<milliseconds>(entry.time).count() % 1000;

			auto escCodeParams = mapLogLevelToEscapeCodeParameters(entry.level);
			std::printf("\x1b[%sm[ %s.%03lli | %s | %s ] %s\n", escCodeParams, timestamp.data(), millisecs, level.data(),
						channel.data(), entry.message.data());
		}

		void quit()
		{
			isAcceptingLogs = false;
			condition.notify_one();
		}
	};

	consteval LogChannel extractChannelFromPath(std::string_view path)
	{
		size_t dirBegin	  = path.rfind(VT_ENGINE_NAME) + sizeof VT_ENGINE_NAME;
		auto pathAfterDir = path.substr(dirBegin);
		size_t dirEnd	  = pathAfterDir.find(std::filesystem::path::preferred_separator);
		auto dir		  = pathAfterDir.substr(0, dirEnd);
		auto channel	  = toEnum<LogChannel>(dir);

		auto restPath	   = pathAfterDir.substr(dirEnd + 1);
		size_t vendorEnd   = restPath.find(std::filesystem::path::preferred_separator);
		auto vendorDir	   = restPath.substr(0, vendorEnd);
		auto vendorChannel = toEnum<LogChannel>(vendorDir);

		return channel.value_or(vendorChannel.value_or(LogChannel::Client));
	}

	export class Log
	{
	public:
		// TODO false compiler error because of consteval
		Log(/*LogChannel channel = extractChannelFromPath(std::source_location::current().file_name())*/) :
			channel(extractChannelFromPath(std::source_location::current().file_name()))
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

		template<typename... Ts> void warning(Ts&&... ts) const
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
