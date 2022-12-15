#include "Mahakam/mhpch.h"
#include "GLFWindow.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Profiler.h"

#include "Mahakam/Events/ApplicationEvent.h"
#include "Mahakam/Events/MouseEvent.h"
#include "Mahakam/Events/KeyEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"

#if MH_PLATFORM_WINDOWS
#include <windows.h>
#endif

#include <stb_image.h>

namespace Mahakam
{
	static bool glfwInitialized = false;

	static void GLFWErrorCallback(int error, const char* message)
	{
		MH_CORE_ERROR("[GLFW Error] (Code {0}): {1}", error, message);
	}

	void GLFWindow::Init(const WindowProps& props)
	{
		MH_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		if (!glfwInitialized)
		{
			int success = glfwInit();
			MH_CORE_ASSERT(success, "Could not initialize GLFW!");

			glfwSetErrorCallback(GLFWErrorCallback);

			glfwInitialized = true;

			int major, minor, rev;

			glfwGetVersion(&major, &minor, &rev);

			MH_CORE_INFO("Initialized GLFW version {0}.{1}.{2}", major, minor, rev);
		}

		MH_CORE_INFO("Creating GLFW window {0} ({1}, {2})", props.Title, props.Width, props.Height);


		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


		// Loading the window icon
		stbi_set_flip_vertically_on_load(0);
		int width, height;
		std::string iconString = props.Iconpath.string();
		unsigned char* pixels = stbi_load(iconString.c_str(), &width, &height, 0, 4);
		GLFWimage* icon = new GLFWimage
		{
			width,
			height,
			pixels
		};


		// Creating the window
#ifndef MH_STANDALONE
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
		m_Window = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
#else
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		width = mode->width;
		height = mode->height;

		m_Window = glfwCreateWindow(width, height, m_Data.Title.c_str(), glfwGetPrimaryMonitor(), nullptr);
#endif

		// Creating the rendering context
		m_Context = RenderingContext::Create(m_Window, (void*)glfwGetProcAddress);
		m_Context->Init();
		glfwSetWindowUserPointer(m_Window, &m_Data);
		if (pixels)
			glfwSetWindowIcon(m_Window, 1, icon);
		SetVSync(false);

		glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

		// Callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);

			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event;

			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.EventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int key)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(key);

			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double posX, double posY)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)posX, (float)posY);
			data.EventCallback(event);
		});
	}

	void GLFWindow::Shutdown()
	{
		MH_PROFILE_FUNCTION();

		glfwDestroyWindow(m_Window);
	}

	GLFWindow::GLFWindow(const WindowProps& props)
	{
		Init(props);
	}

	GLFWindow::~GLFWindow()
	{
		Shutdown();
	}

	void GLFWindow::OnUpdate()
	{
		MH_PROFILE_FUNCTION();

		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void GLFWindow::SetTitle(const std::string& title)
	{
		m_Data.Title = title;

		glfwSetWindowTitle(m_Window, title.c_str());
	}

	void GLFWindow::SetVSync(bool enabled)
	{
		MH_PROFILE_FUNCTION();

		glfwSwapInterval(enabled ? 1 : 0);

		m_Data.VSync = enabled;
	}

	void GLFWindow::SetCursorVisible(bool visible)
	{
		MH_PROFILE_FUNCTION();

		if (visible)
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		m_Data.CursorVisible = visible;
	}
}