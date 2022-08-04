#pragma once

#include "Event.h"

#include "Mahakam/Core/KeyCodes.h"

namespace Mahakam
{
	class KeyEvent : public Event
	{
	protected:
		Key keycode;

		KeyEvent(int keycode) : keycode((Key)keycode) {}

	public:
		inline Key GetKeyCode() const { return keycode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	};


	class KeyPressedEvent : public KeyEvent
	{
	private:
		int repeatCount;

	public:
		KeyPressedEvent(int keycode, int repeatCount) : KeyEvent(keycode), repeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return repeatCount; }

		std::string ToString() const override
		{
			std::stringstream stream;
			stream << GetEventName() << ": " << (int)keycode << " (" << repeatCount << " repeats)";
			return stream.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	};


	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream stream;
			stream << GetEventName() << ": " << (int)keycode;
			return stream.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};


	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode) : KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream stream;
			stream << GetEventName() << ": " << (int)keycode;
			return stream.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}