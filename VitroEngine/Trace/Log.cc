module;
#include "Core/Enum.hh"

#include <bitset>
#include <concepts>
#include <condition_variable>
#include <cstdio>
#include <filesystem>
#include <mutex>
#include <queue>
#include <source_location>
#include <string_view>
export module Vitro.Trace.Log;

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
	concept OverloadsStreamOperator = !std::same_as<bool, T> && !std::is_enum_v<T> && requires(T t, std::ostream& stream)
	{
		stream << t;
	};

	export class Logger : public Singleton<Logger>
	{
		friend class TraceSystem;
		friend class Log;

	public:
		void disableChannel(LogChannel const channel)
		{
			std::lock_guard const lock(mutex);
			size_t const channelIndex = *enum_index(channel);
			disabledChannels.set(channelIndex, true);
		}

		void enableChannel(LogChannel const channel)
		{
			std::lock_guard const lock(mutex);
			size_t const channelIndex = *enum_index(channel);
			disabledChannels.set(channelIndex, false);
		}

		void disableLevel(LogLevel const level)
		{
			std::lock_guard const lock(mutex);
			size_t const levelIndex = *enum_index(level);
			disabledChannels.set(levelIndex, true);
		}

		void enableLevel(LogLevel const level)
		{
			std::lock_guard const lock(mutex);
			size_t const levelIndex = *enum_index(level);
			disabledChannels.set(levelIndex, false);
		}

	private:
		struct Entry
		{
			LogLevel level;
			LogChannel channel;
			std::chrono::system_clock::duration time;
			std::string message;
		};

		std::queue<Entry> queue;
		std::mutex mutex;
		std::condition_variable condition;
		std::bitset<sizeFromEnumMax<LogChannel>()> disabledChannels;
		std::bitset<sizeFromEnumMax<LogLevel>()> disabledLevels;
		std::atomic_bool isAcceptingLogs = true;

		static char const* prepareArgument(bool const arg)
		{
			return arg ? "true" : "false";
		}

		static auto prepareArgument(OverloadsStreamOperator auto&& arg)
		{
			return arg;
		}

		template<typename T> static std::string_view prepareArgument(T const arg) requires std::is_enum_v<T>
		{
			return enum_name(arg);
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
			std::stringstream stream;
			((stream << prepareArgument(std::forward<Ts>(ts)) << ' '), ...);
			return stream.str();
		}

		static std::string makeTimestamp(std::chrono::system_clock::duration const now)
		{
			using namespace std::chrono;
			int64_t const secs = duration_cast<seconds>(now).count();
			std::tm calendarTime;
			localtime_s(&calendarTime, &secs);

			std::string timestamp(sizeof "00:00:00", '\0');
			std::strftime(timestamp.data(), timestamp.capacity(), "%T", &calendarTime);
			return timestamp;
		}

		Logger() = default;

		template<typename... Ts> void submit(LogLevel const level, LogChannel const channel, Ts&&... ts)
		{
			if(isLevelDisabled(level) || isChannelDisabled(channel))
				return;

			enqueue(level, channel, makeMessage(std::forward<Ts>(ts)...));
			condition.notify_one();
		}

		bool isChannelDisabled(LogChannel const channel) const
		{
			size_t const channelIndex = *enum_index(channel);
			return disabledChannels.test(channelIndex);
		}

		bool isLevelDisabled(LogLevel const level) const
		{
			size_t const levelIndex = *enum_index(level);
			return disabledLevels.test(levelIndex);
		}

		void enqueue(LogLevel const level, LogChannel const channel, std::string message)
		{
			auto const now = std::chrono::system_clock::now().time_since_epoch();

			std::lock_guard const lock(mutex);
			queue.emplace(level, channel, now, std::move(message));
		}

		void dispatchLogProcessing()
		{
			while(isAcceptingLogs)
				processQueue();

			while(!queue.empty())
				writeLog(dequeue());
		}

		void processQueue()
		{
			std::unique_lock lock(mutex);
			condition.wait(lock, [&] { return !queue.empty() || !isAcceptingLogs; });

			if(!isAcceptingLogs)
				return;

			auto const entry = dequeue();
			lock.unlock();

			writeLog(entry);
		}

		Entry dequeue()
		{
			auto entry = std::move(queue.front());
			queue.pop();
			return entry;
		}

		void writeLog(Entry const& entry)
		{
			auto const level   = enum_name(entry.level);
			auto const channel = enum_name(entry.channel);

			using namespace std::chrono;
			auto const timestamp	= makeTimestamp(entry.time);
			int64_t const millisecs = duration_cast<milliseconds>(entry.time).count() % 1000;

			auto const escCodeParams = mapLogLevelToEscapeCodeParameters(entry.level);
			std::printf("\x1b[%sm[ %s.%03lli | %s | %s ] %s\n", escCodeParams, timestamp.data(), millisecs, level.data(),
						channel.data(), entry.message.data());
		}

		void quit()
		{
			isAcceptingLogs = false;
			condition.notify_all();
		}
	};

	consteval LogChannel extractChannelFromPath(std::string_view path)
	{
		size_t const dirBegin	= path.rfind(VT_ENGINE_NAME) + sizeof VT_ENGINE_NAME;
		auto const pathAfterDir = path.substr(dirBegin);
		size_t const dirEnd		= pathAfterDir.find(std::filesystem::path::preferred_separator);
		auto const dir			= pathAfterDir.substr(0, dirEnd);
		auto const channel		= enum_cast<LogChannel>(dir);

		auto const restPath		 = pathAfterDir.substr(dirEnd + 1);
		size_t const vendorEnd	 = restPath.find(std::filesystem::path::preferred_separator);
		auto const vendorDir	 = restPath.substr(0, vendorEnd);
		auto const vendorChannel = enum_cast<LogChannel>(vendorDir);

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

		template<typename... Ts> void log(LogLevel const level, Ts&&... ts) const
		{
			Logger::get().submit(level, channel, std::forward<Ts>(ts)...);
		}
	};
}
