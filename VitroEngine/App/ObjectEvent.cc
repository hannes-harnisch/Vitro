module;
#include <format>
#include <string>
#include <type_traits>
export module vt.App.ObjectEvent;

import vt.App.EventSystem;

namespace vt
{
	template<typename T> class ObjectEventSentinel
	{
	protected:
		ObjectEventSentinel()
		{
			EventSystem::notify<ObjectConstructEvent<T>>((T&)*this);
		}

		ObjectEventSentinel(ObjectEventSentinel const& that)
		{
			EventSystem::notify<ObjectCopyConstructEvent<T>>((T&)*this, (T const&)that);
		}

		ObjectEventSentinel(ObjectEventSentinel&& that) noexcept
		{
			EventSystem::notify<ObjectMoveConstructEvent<T>>((T&)*this, (T const&)that);
		}

		~ObjectEventSentinel()
		{
			EventSystem::notify<ObjectDestroyEvent<T>>((T&)*this);
		}

		ObjectEventSentinel& operator=(ObjectEventSentinel const& that)
		{
			EventSystem::notify<ObjectCopyAssignEvent<T>>((T&)*this, (T const&)that);
			return *this;
		}

		ObjectEventSentinel& operator=(ObjectEventSentinel&& that) noexcept
		{
			EventSystem::notify<ObjectMoveAssignEvent<T>>((T&)*this, (T const&)that);
			return *this;
		}
	};

	export template<typename T> class ObjectEventSender : public T, private ObjectEventSentinel<ObjectEventSender<T>>
	{
	public:
		using T::T;
	};

	template<typename T>
	concept InheritsSentinel = std::is_base_of_v<ObjectEventSentinel<T>, T>;

	export template<InheritsSentinel T> struct ObjectConstructEvent
	{
		T& object;

		std::string to_string() const
		{
			auto addr = static_cast<void*>(&object);
			return std::format("Object({})", addr);
		}
	};

	export template<InheritsSentinel T> struct ObjectDestroyEvent
	{
		T& object;

		std::string to_string() const
		{
			auto addr = static_cast<void*>(&object);
			return std::format("Object({})", addr);
		}
	};

	export template<InheritsSentinel T> struct ObjectCopyConstructEvent
	{
		T&		 constructed;
		T const& copied;

		std::string to_string() const
		{
			auto constructed_addr = static_cast<void*>(&constructed);
			auto copied_addr	  = static_cast<void*>(&copied);
			return std::format("Constructed({}), Copied({})", constructed_addr, copied_addr);
		}
	};

	export template<InheritsSentinel T> struct ObjectMoveConstructEvent
	{
		T& constructed;
		T& moved;

		std::string to_string() const
		{
			auto constructed_addr = static_cast<void*>(&constructed);
			auto moved_addr		  = static_cast<void*>(&moved);
			return std::format("Constructed({}), Moved({})", constructed_addr, moved_addr);
		}
	};

	export template<InheritsSentinel T> struct ObjectCopyAssignEvent
	{
		T&		 left;
		T const& right;

		std::string to_string() const
		{
			auto left_addr	= static_cast<void*>(&left);
			auto right_addr = static_cast<void*>(&right);
			return std::format("Left({}), Right({})", left_addr, right_addr);
		}
	};

	export template<InheritsSentinel T> struct ObjectMoveAssignEvent
	{
		T& left;
		T& right;

		std::string to_string() const
		{
			auto left_addr	= static_cast<void*>(&left);
			auto right_addr = static_cast<void*>(&right);
			return std::format("Left({}), Right({})", left_addr, right_addr);
		}
	};
}
