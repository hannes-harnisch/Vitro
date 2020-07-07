#pragma once

#include "Vitro/Client/ClientBase.h"
#include "Vitro/Client/Window.h"
#include "Vitro/Utility/Ref.h"

namespace Vitro
{
	class Engine : public ClientBase
	{
	public:
		static Engine& Get()
		{
			return *Singleton;
		}

		Engine(const std::string& appLogPath, const std::string& engineLogPath);
		virtual ~Engine();

		bool IsRunning()
		{
			return !IsShuttingDown;
		}
		Tick GetTick()
		{
			return EngineTick;
		}

		virtual void OnStart() = 0;

		int Run();

		Engine(const Engine&) = delete;
		Engine(Engine&&)	  = delete;
		Engine& operator=(const Engine&) = delete;
		Engine& operator=(Engine&&) = delete;

	private:
		static inline Engine* Singleton;

		bool ShouldTick {};
		bool IsShuttingDown {};
		bool ResetTickToFirstWindow {};
		Tick EngineTick;
		std::thread LoggingThread;
		std::vector<Ref<Window>> OpenWindows;

		void StartTicking();
		void EraseOpenWindow(Window& window) final override;
		void EmplaceOpenWindow(Window& window) final override;
	};
}
