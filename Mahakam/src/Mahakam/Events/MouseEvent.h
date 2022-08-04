#pragma once

#include "Event.h"

namespace Mahakam
{
	class MouseMovedEvent : public Event
	{
	private:
		float mouseX, mouseY;

	public:
		MouseMovedEvent(float x, float y) : mouseX(x), mouseY(y) {}

		inline float GetX() const { return mouseX; }
		inline float GetY() const { return mouseY; }

		std::string ToString() const override
		{
			std::stringstream stream;
			stream << GetEventName() << ": " << mouseX << ", " << mouseY;
			return stream.str();
		}

		EVENT_CLASS_TYPE(MouseMoved);
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput);
	};


	class MouseScrolledEvent : public Event
	{
	private:
		float offsetX, offsetY;

	public:
		MouseScrolledEvent(float x, float y) : offsetX(x), offsetY(y) {}

		inline float GetX() const { return offsetX; }
		inline float GetY() const { return offsetY; }

		std::string ToString() const override
		{
			std::stringstream stream;
			stream << GetEventName() << ": " << offsetX << ", " << offsetY;
			return stream.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled);
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput);
	};


	class MouseButtonEvent : public Event
	{
	protected:
		MouseButton button;

		MouseButtonEvent(int button) : button((MouseButton)button) {}

	public:
		inline MouseButton GetButton() const { return button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput);
	};


	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream stream;
			stream << GetEventName() << ": " << (int)button;
			return stream.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed);
	};


	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream stream;
			stream << GetEventName() << ": " << (int)button;
			return stream.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased);
	};
}