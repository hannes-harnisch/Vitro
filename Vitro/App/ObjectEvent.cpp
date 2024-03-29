module;
#include <format>
#include <string>
#include <type_traits>
export module vt.App.ObjectEvent;

import vt.App.EventSystem;

namespace vt
{
	// Inherit from this type using CRTP in order to generate events for when an object is constructed, destroyed, copied or
	// moved, such as in order to track all instances of it. Ensure it is the last base class, that T has no additional data
	// members and that T has no relied-on side effects in its special member functions (not counting those of its base
	// classes), because those can only run after the synchronous event dispatch. Only then will calls to all member functions
	// through the events' reference members be safe.
	export template<typename T> class ObjectEventSentinel
	{
	protected:
		ObjectEventSentinel()
		{
			EventSystem::notify<ObjectConstructEvent<T>>(static_cast<T&>(*this));
		}

		ObjectEventSentinel(ObjectEventSentinel const& that)
		{
			EventSystem::notify<ObjectCopyConstructEvent<T>>(static_cast<T&>(*this), static_cast<T const&>(that));
		}

		ObjectEventSentinel(ObjectEventSentinel&& that) noexcept
		{
			EventSystem::notify<ObjectMoveConstructEvent<T>>(static_cast<T&>(*this), static_cast<T&>(that));
		}

		~ObjectEventSentinel()
		{
			EventSystem::notify<ObjectDestroyEvent<T>>(static_cast<T&>(*this));
		}

		ObjectEventSentinel& operator=(ObjectEventSentinel const& that)
		{
			EventSystem::notify<ObjectCopyAssignEvent<T>>(static_cast<T&>(*this), static_cast<T const&>(that));
			return *this;
		}

		ObjectEventSentinel& operator=(ObjectEventSentinel&& that) noexcept
		{
			EventSystem::notify<ObjectMoveAssignEvent<T>>(static_cast<T&>(*this), static_cast<T&>(that));
			return *this;
		}
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
