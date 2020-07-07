#pragma once

#include "Vitro/Utility/Assert.h"

namespace Vitro
{
	// Wrapper for stack-allocated arrays. Will cause stack overflow if stack allocation somehow
	// fails. If too much space is tried to be allocated, it will allocate on the heap. All methods
	// are memory-safe in debug mode.
	template<typename T> class StackArray
	{
	public:
		 StackArray(size_t count) : DataCount(count)
		{
			Data = static_cast<T*>(_malloca(count * sizeof(T)));
		}

		 StackArray(const StackArray& other) : StackArray(other.DataCount)
		{
			auto src = other.begin();
			for(auto dst = begin(); dst != end(); ++src, ++dst)
				*dst = *src;
		}

		 ~StackArray()
		{
			_freea(Data);
		}

		 StackArray& operator=(const StackArray& other)
		{
			return *this = StackArray(other);
		}

		 T& operator[](size_t index)
		{
			AssertDebug(index < DataCount, "Array index out of range: " + std::to_string(index));
			return *(Data + index);
		}

		 const T& operator[](size_t index) const
		{
			AssertDebug(index < DataCount, "Array index out of range: " + std::to_string(index));
			return *(Data + index);
		}

		 size_t Count() const
		{
			return DataCount;
		}

		 size_t ByteSize() const
		{
			return DataCount * sizeof(T);
		}

		__declspec(restrict)  T* Raw()
		{
			return Data;
		}

		__declspec(restrict)  const T* Raw() const
		{
			return Data;
		}

		class Iterator
		{
		public:
#if VTR_DEBUG
			 Iterator(T* pos, T* begin, T* end) : Position(pos), Begin(begin), End(end)
			{}
#else
			 Iterator(T* position) : Position(position)
			{}
#endif

			 T& operator*()
			{
				return *Position;
			}

			 T* operator->()
			{
				return Position;
			}

			 bool operator==(Iterator other)
			{
				return Position == other.Position;
			}

			 bool operator!=(Iterator other)
			{
				return Position != other.Position;
			}

			 Iterator& operator++()
			{
				AssertDebug(Position < End, "Cannot increment iterator past end.");
				Position += 1;
				return *this;
			}

			 Iterator& operator--()
			{
				AssertDebug(Begin < Position, "Cannot decrement iterator before begin.");
				Position -= 1;
				return *this;
			}

		private:
			T* Position;
#if VTR_DEBUG
			T* Begin;
			T* End;
#endif
		};

		class ConstIterator
		{
		public:
#if VTR_DEBUG
			 ConstIterator(T* pos, T* begin, T* end) : Position(pos), Begin(begin), End(end)
			{}
#else
			 ConstIterator(T* position) : Position(position)
			{}
#endif

			 const T& operator*() const
			{
				return *Position;
			}

			 const T* operator->() const
			{
				return Position;
			}

			 bool operator==(ConstIterator other) const
			{
				return Position == other.Position;
			}

			 bool operator!=(ConstIterator other) const
			{
				return Position != other.Position;
			}

			 ConstIterator& operator++()
			{
				AssertDebug(Position < End, "Cannot increment iterator past end.");
				Position += 1;
				return *this;
			}

			 ConstIterator& operator--()
			{
				AssertDebug(Begin < Position, "Cannot decrement iterator before begin.");
				Position -= 1;
				return *this;
			}

		private:
			T* Position;
#if VTR_DEBUG
			T* Begin;
			T* End;
#endif
		};

		 Iterator begin()
		{
#if VTR_DEBUG
			return Iterator(Data, Data, Data + DataCount);
#else
			return Iterator(Data);
#endif
		}

		 ConstIterator begin() const
		{
#if VTR_DEBUG
			return ConstIterator(Data, Data, Data + DataCount);
#else
			return ConstIterator(Data);
#endif
		}

		 Iterator end()
		{
#if VTR_DEBUG
			return Iterator(Data + DataCount, Data, Data + DataCount);
#else
			return Iterator(Data + DataCount);
#endif
		}

		 ConstIterator end() const
		{
#if VTR_DEBUG
			return ConstIterator(Data + DataCount, Data, Data + DataCount);
#else
			return ConstIterator(Data + DataCount);
#endif
		}

		StackArray(StackArray&&) = delete;
		StackArray& operator=(StackArray&&) = delete;
		void* operator new(size_t)			= delete;
		void* operator new[](size_t)		= delete;
		void operator delete(void*)			= delete;
		void operator delete[](void*)		= delete;

	private:
		T* __restrict Data;
		size_t DataCount;
	};
}
