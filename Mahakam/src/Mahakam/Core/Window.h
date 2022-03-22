#pragma once
#include "mhpch.h"
#include "Core.h"
#include "Mahakam/Events/Event.h"

#include "Mahakam/Renderer/RenderingContext.h"

namespace Mahakam
{
	struct WindowProps
	{
		std::string title;
		std::string iconpath;
		uint32_t width, height;

		WindowProps(const std::string& title = "Mahakam", const std::string& iconpath = "", uint32_t width = 1600, uint32_t height = 900)
			: title(title), iconpath(iconpath), width(width), height(height) {}
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual double GetTime() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void SetCursorVisible(bool visible) = 0;

		virtual void* GetNativeWindow() const = 0;

		virtual void* GetProcess() const = 0;

		virtual RenderingContext* GetContext() const = 0;

		static Scope<Window> Create(const WindowProps& props = WindowProps());
	};
}