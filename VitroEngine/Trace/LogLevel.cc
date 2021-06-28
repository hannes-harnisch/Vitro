export module Vitro.Trace.LogLevel;

namespace vt
{
	export enum class LogLevel : unsigned char {
		Verbose,
		Debug,
		Info,
		Warning,
		Error,
		Fatal,
	};
}
