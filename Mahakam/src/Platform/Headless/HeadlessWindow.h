#pragma once

#include "Mahakam/Core/Window.h"
#include "Mahakam/Renderer/RenderingContext.h"

#include <chrono>

namespace Mahakam
{
	class HeadlessWindow : public Window
	{
	private:
		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync, CursorVisible;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
        std::chrono::steady_clock::time_point m_Start;

		Scope<RenderingContext> m_Context;

	public:
		HeadlessWindow(const WindowProps& props);
		virtual ~HeadlessWindow();

		virtual void OnUpdate() override;

		virtual inline double GetTime() override { return std::chrono::time_point_cast<std::chrono::milliseconds>(m_Start).time_since_epoch().count() * 0.001; }

		virtual inline unsigned int GetWidth() const override { return m_Data.Width; }
		virtual inline unsigned int GetHeight() const override { return m_Data.Height; }

		virtual inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

		virtual void SetTitle(const std::string& title) override;

		virtual void SetVSync(bool enabled) override;
		virtual inline bool IsVSync() const override { return m_Data.VSync; }

		virtual void SetCursorVisible(bool visible) override;
		virtual bool IsCursorVisible() const override { return m_Data.CursorVisible; }

		virtual inline void* GetNativeWindow() const override { return (void*)this; }

		virtual inline void* GetProcess() const override { return (void*)this; }

		virtual inline RenderingContext& GetContext() const override { return *m_Context; }

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	};
}