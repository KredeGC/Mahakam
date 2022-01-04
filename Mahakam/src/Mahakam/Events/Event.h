#pragma once

#include "mhpch.h"
#include "Mahakam/Core/Core.h"

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
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};


#define EVENT_CLASS_TYPE(type) static EventType getStaticType() { return EventType::##type; } \
	virtual EventType getEventType() const override { return getStaticType(); } \
	virtual const char* getEventName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int getCategoryFlags() const override { return category; }


	class Event
	{
	private:
		friend class EventDispatcher;

	public:
		virtual EventType getEventType() const = 0;
		virtual const char* getEventName() const = 0;
		virtual int getCategoryFlags() const = 0;
		virtual std::string toString() const { return getEventName(); }

		inline bool isInCategory(EventCategory category)
		{
			return getCategoryFlags() & category;
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
		bool dispatchEvent(EventFn<T> func)
		{
			if (event.getEventType() == T::getStaticType())
			{
				event.handled = func(*(T*)&event);
				return true;
			}
			return false;
		}
	};

	inline std::ostream& operator<<(std::ostream& stream, const Event& event)
	{
		return stream << event.toString();
	}
}