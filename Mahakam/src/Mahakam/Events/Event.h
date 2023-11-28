#pragma once
#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/Types.h"

#include <sstream>
#include <string>
#include <functional>

namespace Mahakam
{
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication	= Bit(0),
		EventCategoryInput			= Bit(1),
		EventCategoryKeyboard		= Bit(2),
		EventCategoryMouse			= Bit(3),
		EventCategoryMouseButton	= Bit(4)
	};


#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; } \
	virtual EventType GetEventType() const override { return GetStaticType(); } \
	virtual const char* GetEventName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }


	class Event
	{
	private:
		friend class EventDispatcher;

	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetEventName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetEventName(); }

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}

		bool handled = false;
	};


	class EventDispatcher
	{
	private:
		Event& event;

		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event) : event(event) {}

		template<typename T>
		bool DispatchEvent(EventFn<T> func)
		{
			if (event.GetEventType() == T::GetStaticType())
			{
				event.handled |= func(static_cast<T&>(event));
				return true;
			}
			return false;
		}
	};

	inline std::ostream& operator<<(std::ostream& stream, const Event& event)
	{
		return stream << event.ToString();
	}
}