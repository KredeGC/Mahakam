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

	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return data.width; }
		inline unsigned int GetHeight() const override { return data.height; }

		inline void SetEventCallback(const EventCallbackFn& callback) override { data.eventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		virtual void SetCursorVisible(bool visible) override;

		inline virtual void* GetNativeWindow() const override { return window; }

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	};
}