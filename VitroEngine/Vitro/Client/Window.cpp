#include "_pch.h"
#include "Window.h"

#include "Vitro/Client/WindowEvent.h"
#include "Vitro/Graphics/Renderer3D.h"

#if VTR_SYSTEM_WINDOWS
	#include "Vitro/API/Windows/Window.h"
#endif
#include <Vitro/Utility/Log.h>

namespace Vitro
{
	Ref<Window> Window::New(int width, int height, int x, int y, const std::string& title)
	{
#if VTR_SYSTEM_WINDOWS
		return Ref<Windows::Window>::New(width, height, x, y, title);
#else
	#error Unsupported system.
#endif
	}

	void Window::OnTick(Tick t)
	{
		Renderer->BeginScene();
		for(auto layer : LayerStack)
			if(layer->Enabled)
				layer->OnTick(t);
		Renderer->EndScene();
	}

	void Window::OnEvent(Event& e)
	{
		LogDebug(e);
		for(auto layer = LayerStack.rbegin(); layer != LayerStack.rend(); layer++)
		{
			if((*layer)->Enabled)
				(*layer)->OnEvent(e);
			if(e.IsHandled())
				break;
		}
	}

	void Window::Detach(Layer& layer)
	{
		auto i = std::find(LayerStack.begin(), LayerStack.begin() + LastLayerIndex, &layer);
		if(i != LayerStack.begin() + LastLayerIndex)
		{
			layer.OnDetach();
			LayerStack.erase(i);
			LastLayerIndex--;
			delete &layer;
		}
	}

	void Window::Detach(Overlay& overlay)
	{
		auto i = std::find(LayerStack.begin() + LastLayerIndex, LayerStack.end(), &overlay);
		if(i != LayerStack.end())
		{
			overlay.OnDetach();
			LayerStack.erase(i);
			delete &overlay;
		}
	}

	Window::Window(Window&& other) noexcept :
		Renderer(std::move(other.Renderer)),
		LayerStack(std::move(other.LayerStack)),
		LastLayerIndex(other.LastLayerIndex)
	{}

	Window::~Window()
	{
		for(auto layer : LayerStack)
		{
			layer->OnDetach();
			delete layer;
		}
		LayerStack.clear();
	}

	Window& Window::operator=(Window&& other) noexcept
	{
		std::swap(Renderer, other.Renderer);
		std::swap(LayerStack, other.LayerStack);
		std::swap(LastLayerIndex, other.LastLayerIndex);
		return *this;
	}

	Layer& Window::Attach(Layer& layer)
	{
		layer.Renderer = Renderer;
		layer.OnAttach();
		LayerStack.emplace(LayerStack.begin() + LastLayerIndex, &layer);
		LastLayerIndex++;
		return layer;
	}

	Overlay& Window::Attach(Overlay& overlay)
	{
		overlay.Renderer = Renderer;
		overlay.OnAttach();
		LayerStack.emplace_back(&overlay);
		return overlay;
	}
}
