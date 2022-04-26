#include "mhpch.h"
#include "WindowsWindow.h"

#include "Mahakam/Events/ApplicationEvent.h"
#include "Mahakam/Events/MouseEvent.h"
#include "Mahakam/Events/KeyEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"

#include <stb_image.h>

namespace Mahakam {

	static bool glfwInitialized = false;

	static void GLFWErrorCallback(int error, const char* message)
	{
		MH_CORE_ERROR("[GLFW Error] (Code {0}): {1}", error, message);
	}

	Scope<Window> Window::Create(const WindowProps& props)
	{
		return CreateScope<WindowsWindow>(props);
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		MH_PROFILE_FUNCTION();

#ifdef MH_RUNTIME
		FreeConsole();
#endif

		data.title = props.title;
		data.width = props.width;
		data.height = props.height;

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

		MH_CORE_INFO("Creating GLFW window {0} ({1}, {2})", props.title, props.width, props.height);


		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


		// Loading the window icon
		stbi_set_flip_vertically_on_load(0);
		int width, height;
		unsigned char* pixels = stbi_load(props.iconpath.c_str(), &width, &height, 0, 4);
		GLFWimage* icon = new GLFWimage
		{
			width,
			height,
			pixels
		};


		// Creating the window
		window = glfwCreateWindow((int)data.width, (int)data.height, data.title.c_str(), nullptr, nullptr);
		context = RenderingContext::Create(window, (void*)glfwGetProcAddress);
		context->Init();
		glfwSetWindowUserPointer(window, &data);
		if (pixels)
			glfwSetWindowIcon(window, 1, icon);
		SetVSync(false);

		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

		// Callbacks
		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);

			data.eventCallback(event);
		});

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event;

			data.eventCallback(event);
		});

		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.eventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.eventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.eventCallback(event);
				break;
			}
			}
		});

		glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int key)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(key);

			data.eventCallback(event);
		});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.eventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.eventCallback(event);
				break;
			}
			}
		});

		glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.eventCallback(event);
		});

		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double posX, double posY)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)posX, (float)posY);
			data.eventCallback(event);
		});
	}

	void WindowsWindow::Shutdown()
	{
		MH_PROFILE_FUNCTION();

		glfwDestroyWindow(window);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::OnUpdate()
	{
		MH_PROFILE_FUNCTION();

		glfwPollEvents();
		context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		MH_PROFILE_FUNCTION();

		glfwSwapInterval(enabled ? 1 : 0);

		data.vsync = enabled;
	}

	void WindowsWindow::SetCursorVisible(bool visible)
	{
		MH_PROFILE_FUNCTION();

		if (visible)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		data.cursorVisible = visible;
	}
}