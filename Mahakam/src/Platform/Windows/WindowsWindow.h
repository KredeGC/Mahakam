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
			std::string Title;
			unsigned int Width, Height;
			bool VSync, CursorVisible;

			EventCallbackFn EventCallback;
		};

		GLFWwindow* m_Window;
		WindowData m_Data;

		RenderingContext* m_Context;

	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		virtual void OnUpdate() override;

		virtual inline double GetTime() override { return glfwGetTime(); }

		virtual inline unsigned int GetWidth() const override { return m_Data.Width; }
		virtual inline unsigned int GetHeight() const override { return m_Data.Height; }

		virtual inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

		virtual void SetVSync(bool enabled) override;
		virtual inline bool IsVSync() const override { return m_Data.VSync; }

		virtual void SetCursorVisible(bool visible) override;
		virtual bool IsCursorVisible() const override { return m_Data.CursorVisible; }

		virtual inline void* GetNativeWindow() const override { return m_Window; }

		virtual inline void* GetProcess() const override { return (void*)glfwGetProcAddress; }

		virtual inline RenderingContext* GetContext() const override { return m_Context; }

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	};
}