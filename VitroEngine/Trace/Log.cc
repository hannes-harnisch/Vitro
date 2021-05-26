module;
#include "Core/Enum.hh"

#include <bitset>
#include <concepts>
#include <condition_variable>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <queue>
#include <source_location>
#include <string_view>
export module Vitro.Trace.Log;

import Vitro.Core.Singleton;
import Vitro.Trace.Console;
import Vitro.Trace.LogLevel;

template<typename T> concept OverloadsArrowOperator = requires(T t)
{
	t->toString();
};

template<typename T>
concept OverloadsStreamOperator = !std::same_as<bool, T> && !std::is_enum_v<T> && requires(T t, std::ostream& stream)
{
	stream << t;
};

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
	Direct3D,
	Vulkan
};

export class Logger : public Singleton<Logger>
{
	friend class TraceSystem;
	friend class Log;

public:
	void disableChannel(LogChannel channel)
	{
		const std::lock_guard lock(mutex);
		const size_t channelIndex = *enum_index(channel);
		disabledChannels.set(channelIndex, true);
	}

	void enableChannel(LogChannel channel)
	{
		const std::lock_guard lock(mutex);
		const size_t channelIndex = *enum_index(channel);
		disabledChannels.set(channelIndex, false);
	}

	void disableLevel(LogLevel level)
	{
		const std::lock_guard lock(mutex);
		const size_t levelIndex = *enum_index(level);
		disabledChannels.set(levelIndex, true);
	}

	void enableLevel(LogLevel level)
	{
		const std::lock_guard lock(mutex);
		const size_t levelIndex = *enum_index(level);
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

	static const char* prepareArgument(bool arg)
	{
		return arg ? "true" : "false";
	}

	static auto prepareArgument(OverloadsStreamOperator auto&& arg)
	{
		return arg;
	}

	template<typename T> static std::string_view prepareArgument(T arg) requires std::is_enum_v<T>
	{
		return enum_name(arg);
	}

	static std::string prepareArgument(OverloadsArrowOperator auto&& arg)
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

	static std::string makeTimestamp(std::chrono::system_clock::duration now)
	{
		using namespace std::chrono;
		const int64_t secs = duration_cast<seconds>(now).count();
		std::tm calendarTime;
		localtime_s(&calendarTime, &secs);

		std::string timestamp(sizeof "00:00:00", '\0');
		std::strftime(timestamp.data(), timestamp.capacity(), "%T", &calendarTime);
		return timestamp;
	}

	Logger() = default;

	template<typename... Ts> void submit(LogLevel level, LogChannel channel, Ts&&... ts)
	{
		if(isLevelDisabled(level) || isChannelDisabled(channel))
			return;

		enqueue(level, channel, makeMessage(std::forward<Ts>(ts)...));
		condition.notify_one();
	}

	bool isChannelDisabled(LogChannel channel) const
	{
		const size_t channelIndex = *enum_index(channel);
		return disabledChannels.test(channelIndex);
	}

	bool isLevelDisabled(LogLevel level) const
	{
		const size_t levelIndex = *enum_index(level);
		return disabledLevels.test(levelIndex);
	}

	void enqueue(LogLevel level, LogChannel channel, std::string message)
	{
		auto now = std::chrono::system_clock::now().time_since_epoch();

		const std::lock_guard lock(mutex);
		queue.emplace(level, channel, now, message);
	}

	void runQueueProcessing()
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

		const Entry entry = dequeue();
		lock.unlock();

		writeLog(entry);
	}

	Entry dequeue()
	{
		const auto entry = std::move(queue.front());
		queue.pop();
		return entry;
	}

	void writeLog(const Entry& entry)
	{
		const auto level   = enum_name(entry.level);
		const auto channel = enum_name(entry.channel);

		using namespace std::chrono;
		const auto timestamp	= makeTimestamp(entry.time);
		const int64_t millisecs = duration_cast<milliseconds>(entry.time).count() % 1000;

		Console::get().setTextColor(entry.level);
		std::printf("[ %s.%03lli | %s | %s ] %s\n", timestamp.data(), millisecs, level.data(), channel.data(),
					entry.message.data());
	}

	void quit()
	{
		isAcceptingLogs = false;
		condition.notify_all();
	}
};

consteval LogChannel extractChannelFromPath(std::string_view path)
{
	const size_t dirBegin	= path.rfind(VE_ENGINE_NAME) + sizeof VE_ENGINE_NAME;
	const auto pathAfterDir = path.substr(dirBegin);
	const size_t dirEnd		= pathAfterDir.find(std::filesystem::path::preferred_separator);
	const auto dir			= pathAfterDir.substr(0, dirEnd);
	const auto channel		= enum_cast<LogChannel>(dir);

	const auto restPath		 = pathAfterDir.substr(dirEnd + 1);
	const size_t vendorEnd	 = restPath.find(std::filesystem::path::preferred_separator);
	const auto vendorDir	 = restPath.substr(0, vendorEnd);
	const auto vendorChannel = enum_cast<LogChannel>(vendorDir);

	return channel.value_or(vendorChannel.value_or(LogChannel::Client));
}

export class Log
{
public:
	// TODO: change to default parameter once stable
	Log() : channel(extractChannelFromPath(std::source_location::current().file_name()))
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
