#pragma once
#include "mhpch.h"
#include "Core.h"
#include "Mahakam/Events/Event.h"

namespace Mahakam
{
	struct WindowProps
	{
		std::string title;
		uint32_t width, height;

		WindowProps(const std::string& title = "Mahakam", uint32_t width = 1600, uint32_t height = 900)
			: title(title), width(width), height(height) {}
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		//Window(int width, int height, const char* title);
		virtual ~Window() {}

		virtual void onUpdate() = 0;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

		virtual void setEventCallback(const EventCallbackFn& callback) = 0;
		virtual void setVSync(bool enabled) = 0;
		virtual bool isVSync() const = 0;

		virtual void setCursorVisible(bool visible) const = 0;

		virtual void* getNativeWindow() const = 0;

		static Window* create(const WindowProps& props = WindowProps());
	};
}