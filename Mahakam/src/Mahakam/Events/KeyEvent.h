#pragma once

#include "Event.h"

namespace Mahakam
{
	class KeyEvent : public Event
	{
	protected:
		int keycode;

		KeyEvent(int keycode) : keycode(keycode) {}

	public:
		inline int getKeyCode() const { return keycode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	};


	class KeyPressedEvent : public KeyEvent
	{
	private:
		int repeatCount;

	public:
		KeyPressedEvent(int keycode, int repeatCount) : KeyEvent(keycode), repeatCount(repeatCount) {}

		inline int getRepeatCount() const { return repeatCount; }

		std::string toString() const override
		{
			std::stringstream stream;
			stream << getEventName() << ": " << keycode << " (" << repeatCount << " repeats)";
			return stream.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	};


	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}

		std::string toString() const override
		{
			std::stringstream stream;
			stream << getEventName() << ": " << keycode;
			return stream.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};


	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode) : KeyEvent(keycode) {}

		std::string toString() const override
		{
			std::stringstream stream;
			stream << getEventName() << ": " << keycode;
			return stream.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}