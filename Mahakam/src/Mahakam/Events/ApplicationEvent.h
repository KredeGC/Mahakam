#pragma once

#include "Event.h"

namespace Mahakam
{
	class WindowResizeEvent : public Event
	{
	private:
		unsigned int width, height;

	public:
		WindowResizeEvent(unsigned int width, unsigned int height) : width(width), height(height) {}

		inline unsigned int getWidth() { return width; }
		inline unsigned int getHeight() { return height; }

		std::string toString() const override
		{
			std::stringstream stream;
			stream << getEventName() << ": " << width << ", " << height;
			return stream.str();
		}

		EVENT_CLASS_TYPE(WindowResize);
		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	};


	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose);
		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	};


	class WindowAppTickEvent : public Event
	{
	public:
		WindowAppTickEvent() {}

		EVENT_CLASS_TYPE(AppTick);
		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	};


	class WindowAppUpdateEvent : public Event
	{
	public:
		WindowAppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate);
		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	};


	class WindowAppRenderEvent : public Event
	{
	public:
		WindowAppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender);
		EVENT_CLASS_CATEGORY(EventCategoryApplication);
	};
}