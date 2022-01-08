#pragma once

#include "Mahakam/Core/Window.h"
#include "Mahakam/Renderer/RenderingContext.h"

#include <GLFW/glfw3.h>

namespace Mahakam
{
	class WindowsWindow : public Window
	{
	private:
		struct WindowData
		{
			std::string title;
			unsigned int width, height;
			bool vsync, cursorVisible;

			EventCallbackFn eventCallback;
		};

		GLFWwindow* window;
		WindowData data;

		RenderingContext* context;

		virtual void init(const WindowProps& props);
		virtual void shutdown();

	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void onUpdate() override;

		inline unsigned int getWidth() const override { return data.width; }
		inline unsigned int getHeight() const override { return data.height; }

		inline void setEventCallback(const EventCallbackFn& callback) override { data.eventCallback = callback; }
		void setVSync(bool enabled) override;
		bool isVSync() const override;

		virtual void setCursorVisible(bool visible) override;

		inline virtual void* getNativeWindow() const override { return window; }
	};
}