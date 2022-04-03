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

		virtual void OnUpdate() override;

		virtual inline double GetTime() override { return glfwGetTime(); }

		virtual inline unsigned int GetWidth() const override { return data.width; }
		virtual inline unsigned int GetHeight() const override { return data.height; }

		virtual inline void SetEventCallback(const EventCallbackFn& callback) override { data.eventCallback = callback; }

		virtual void SetVSync(bool enabled) override;
		virtual inline bool IsVSync() const override { return data.vsync; }

		virtual void SetCursorVisible(bool visible) override;
		virtual bool IsCursorVisible() const override { return data.cursorVisible; }

		virtual inline void* GetNativeWindow() const override { return window; }

		virtual inline void* GetProcess() const override { return glfwGetProcAddress; }

		virtual inline RenderingContext* GetContext() const override { return context; }

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	};
}