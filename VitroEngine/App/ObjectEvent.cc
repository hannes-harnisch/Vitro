module;
#include <concepts>
export module vt.App.ObjectEvent;

import vt.App.Event;
import vt.App.EventSystem;

namespace vt
{
	export template<typename T> class ObjectEventSource
	{
	public:
		ObjectEventSource()
		{
			EventSystem::notify<ObjectConstructEvent<T>>(cast(*this));
		}

		ObjectEventSource(ObjectEventSource const& that)
		{
			EventSystem::notify<ObjectCopyConstructEvent<T>>(cast(*this), cast(that));
		}

		ObjectEventSource(ObjectEventSource&& that) noexcept
		{
			EventSystem::notify<ObjectMoveConstructEvent<T>>(cast(*this), cast(that));
		}

		~ObjectEventSource()
		{
			EventSystem::notify<ObjectDestroyEvent<T>>(cast(*this));
		}

		ObjectEventSource& operator=(ObjectEventSource const& that)
		{
			EventSystem::notify<ObjectCopyAssignEvent<T>>(cast(*this), cast(that));
			return *this;
		}

		ObjectEventSource& operator=(ObjectEventSource&& that) noexcept
		{
			EventSystem::notify<ObjectMoveAssignEvent<T>>(cast(*this), cast(that));
			return *this;
		}

	private:
		static T& cast(ObjectEventSource& src)
		{
			return static_cast<T&>(src);
		}

		static T const& cast(ObjectEventSource const& src)
		{
			return static_cast<T const&>(src);
		}
	};

	export template<typename T>
	requires std::derived_from<T, ObjectEventSource<T>>
	class ObjectConstructEvent final : public Event
	{
	public:
		T& object;

		ObjectConstructEvent(T& object) : object(object)
		{}
	};

	export template<typename T>
	requires std::derived_from<T, ObjectEventSource<T>>
	class ObjectDestroyEvent final : public Event
	{
	public:
		T& object;

		ObjectDestroyEvent(T& object) : object(object)
		{}
	};

	export template<typename T>
	requires std::derived_from<T, ObjectEventSource<T>>
	class ObjectCopyConstructEvent final : public Event
	{
	public:
		T&		 constructed;
		T const& copied;

		ObjectCopyConstructEvent(T& constructed, T const& copied) : constructed(constructed), copied(copied)
		{}
	};

	export template<typename T>
	requires std::derived_from<T, ObjectEventSource<T>>
	class ObjectMoveConstructEvent final : public Event
	{
	public:
		T& constructed;
		T& moved;

		ObjectMoveConstructEvent(T& constructed, T& moved) : constructed(constructed), moved(moved)
		{}
	};

	export template<typename T>
	requires std::derived_from<T, ObjectEventSource<T>>
	class ObjectCopyAssignEvent final : public Event
	{
	public:
		T&		 left;
		T const& right;

		ObjectCopyAssignEvent(T& left, T const& right) : left(left), right(right)
		{}
	};

	export template<typename T>
	requires std::derived_from<T, ObjectEventSource<T>>
	class ObjectMoveAssignEvent final : public Event
	{
	public:
		T& left;
		T& right;

		ObjectMoveAssignEvent(T& left, T& right) : left(left), right(right)
		{}
	};
}
