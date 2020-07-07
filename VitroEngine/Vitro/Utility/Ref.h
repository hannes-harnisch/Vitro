#pragma once

namespace Vitro
{
	class RefCounted
	{
		template<typename T> friend class Ref;

		mutable uint32_t RefCount = 0;
	};

	template<typename T> class Ref final
	{
		static_assert(std::has_virtual_destructor_v<T>, "Template parameter must have a virtual destructor.");

		template<class D> friend class Ref;

	public:
		template<typename... Args> static Ref New(Args&&... args)
		{
			return Ref(new T(std::forward<Args>(args)...));
		}

		Ref() : Pointer(nullptr)
		{}
		Ref(std::nullptr_t) : Pointer(nullptr)
		{}

		Ref(T* ptr) : Pointer(ptr)
		{
			static_assert(std::is_base_of_v<RefCounted, T>, "Template parameter must inherit from Vitro::RefCounted.");
			IncrementRef();
		}

		Ref(const Ref& other) : Pointer(other.Pointer)
		{
			IncrementRef();
		}

		template<typename D> Ref(const Ref<D>& other) : Pointer(other.Pointer)
		{
			static_assert(std::is_base_of_v<T, D>, "Type must derive from target type.");
			IncrementRef();
		}

		Ref(Ref&& other) noexcept : Pointer(std::exchange(other.Pointer, nullptr))
		{}

		template<typename D> Ref(Ref<D>&& other) noexcept : Pointer(std::exchange(other.Pointer, nullptr))
		{
			static_assert(std::is_base_of_v<T, D>, "Type must derive from target type.");
		}

		~Ref()
		{
			DecrementRef();
		}

		Ref& operator=(std::nullptr_t)
		{
			DecrementRef();
			Pointer = nullptr;
			return *this;
		}

		Ref& operator=(const Ref& other)
		{
			other.IncrementRef();
			DecrementRef();
			Pointer = other.Pointer;
			return *this;
		}

		Ref& operator=(Ref&& other) noexcept
		{
			DecrementRef();
			Pointer = std::exchange(other.Pointer, nullptr);
			return *this;
		}

		T* operator->()
		{
			return Pointer;
		}
		const T* operator->() const
		{
			return Pointer;
		}

		T& operator*()
		{
			return *Pointer;
		}
		const T& operator*() const
		{
			return *Pointer;
		}

		T** operator&()
		{
			return &Pointer;
		}
		T* const* operator&() const
		{
			return &Pointer;
		}

		bool operator==(Ref other) const
		{
			return Pointer == other.Pointer;
		}
		bool operator!=(Ref other) const
		{
			return Pointer != other.Pointer;
		}

		operator T*()
		{
			return Pointer;
		}
		operator const T*() const
		{
			return Pointer;
		}
		operator bool() const
		{
			return Pointer != nullptr;
		}

	private:
		T* Pointer;

		void IncrementRef() const
		{
			if(Pointer)
				Pointer->RefCount++;
		}

		void DecrementRef() const
		{
			if(Pointer)
			{
				Pointer->RefCount--;
				if(Pointer->RefCount == 0)
					delete Pointer;
			}
		}
	};
}
