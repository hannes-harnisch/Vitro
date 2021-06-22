module;
#include "Windows.API.hh"

#include <cstdint>
export module Vitro.Windows.Terminal;

import Vitro.Trace.LogLevel;

namespace Windows
{
	export class Terminal
	{
	public:
		constexpr static uint8_t Black	= 0;
		constexpr static uint8_t Navy	= 1;
		constexpr static uint8_t Green	= 2;
		constexpr static uint8_t Teal	= 3;
		constexpr static uint8_t Maroon = 4;
		constexpr static uint8_t Purple = 5;
		constexpr static uint8_t Olive	= 6;
		constexpr static uint8_t Silver = 7;
		constexpr static uint8_t Gray	= 8;
		constexpr static uint8_t Blue	= 9;
		constexpr static uint8_t Lime	= 10;
		constexpr static uint8_t Aqua	= 11;
		constexpr static uint8_t Red	= 12;
		constexpr static uint8_t Pink	= 13;
		constexpr static uint8_t Yellow = 14;
		constexpr static uint8_t White	= 15;

		void setTextColor(LogLevel const level)
		{
			::SetConsoleTextAttribute(standardOut, getColorMaskFromLogLevel(level));
		}

	protected:
		Terminal() : standardOut(::GetStdHandle(STD_OUTPUT_HANDLE))
		{}

	private:
		HANDLE const standardOut;

		static consteval uint16_t makeTextColorMask(uint8_t const foreground, uint8_t const background)
		{
			return (foreground * 16) | background;
		}

		static uint16_t getColorMaskFromLogLevel(LogLevel const level)
		{
			switch(level)
			{
				case LogLevel::Verbose: return makeTextColorMask(Black, Silver);
				case LogLevel::Debug: return makeTextColorMask(Black, Aqua);
				case LogLevel::Info: return makeTextColorMask(Black, Lime);
				case LogLevel::Warning: return makeTextColorMask(Olive, White);
				case LogLevel::Error: return makeTextColorMask(Maroon, White);
				case LogLevel::Fatal: return makeTextColorMask(Red, White);
			}
			return makeTextColorMask(Black, White);
		}
	};
}