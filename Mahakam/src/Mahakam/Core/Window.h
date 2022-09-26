#pragma once
#include "Core.h"

#include "SharedLibrary.h"

namespace Mahakam
{
	class Event;
	class RenderingContext;

	struct WindowProps
	{
		std::string Title;
		std::string Iconpath;
		uint32_t Width, Height;

		WindowProps(const std::string& title = "Mahakam", const std::string& iconpath = "", uint32_t width = 1600, uint32_t height = 900)
			: Title(title), Iconpath(iconpath), Width(width), Height(height) {}
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
		virtual bool IsCursorVisible() const = 0;

		virtual void* GetNativeWindow() const = 0;

		virtual void* GetProcess() const = 0;

		virtual RenderingContext& GetContext() const = 0;

		inline static Scope<Window> Create(const WindowProps& props = WindowProps()) { return CreateImpl(props); }

	private:
		MH_DECLARE_FUNC(CreateImpl, Scope<Window>, const WindowProps& props);
	};
}