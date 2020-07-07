#pragma once

#include "Vitro/Client/Layer.h"

namespace Vitro
{
	class Editor : public Overlay
	{
	public:
		Editor(void* nativeWindowHandle);
		~Editor();

		void OnAttach() override;
		void OnDetach() override;
		void OnTick(Tick t) override;
		void OnEvent(Event& e) override;

	private:
		void BeginFrame();
		void EndFrame();
	};
}
