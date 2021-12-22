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

		inline float getX() const { return mouseX; }
		inline float getY() const { return mouseY; }

		std::string toString() const override
		{
			std::stringstream stream;
			stream << getEventName() << ": " << mouseX << ", " << mouseY;
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

		inline float getX() const { return offsetX; }
		inline float getY() const { return offsetY; }

		std::string toString() const override
		{
			std::stringstream stream;
			stream << getEventName() << ": " << offsetX << ", " << offsetY;
			return stream.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled);
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput);
	};


	class MouseButtonEvent : public Event
	{
	protected:
		int button;

		MouseButtonEvent(int button) : button(button) {}

	public:
		inline int getButton() const { return button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput);
	};


	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

		inline int getButton() const { return button; }

		std::string toString() const override
		{
			std::stringstream stream;
			stream << getEventName() << ": " << button;
			return stream.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed);
	};


	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

		inline int getButton() const { return button; }

		std::string toString() const override
		{
			std::stringstream stream;
			stream << getEventName() << ": " << button;
			return stream.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased);
	};
}