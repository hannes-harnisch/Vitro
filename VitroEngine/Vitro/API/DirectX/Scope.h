#pragma once

namespace Vitro::DirectX
{
	template<typename T> class Scope final
	{
	public:
		Scope() : Pointer(nullptr)
		{}

		Scope(Scope&& other) noexcept : Pointer(std::exchange(other.Pointer, nullptr))
		{}

		~Scope()
		{
			release();
		}

		Scope& operator=(Scope&& other) noexcept
		{
			std::swap(Pointer, other.Pointer);
			return *this;
		}

		T* operator->() const
		{
			return Pointer;
		}

		T& operator*() const
		{
			return *Pointer;
		}

		T** operator&()
		{
			// release();
			return &Pointer;
		}

		T* const* operator&() const
		{
			return &Pointer;
		}

		bool operator==(Scope other) const
		{
			return Pointer == other.Pointer;
		}

		bool operator!=(Scope other) const
		{
			return Pointer != other.Pointer;
		}

		operator T*() const
		{
			return Pointer;
		}

		operator bool() const
		{
			return Pointer != nullptr;
		}

	private:
		T* Pointer;

		void release() noexcept
		{
			if(Pointer)
				Pointer->Release();
		}
	};
}
