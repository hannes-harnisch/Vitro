module;
#include <iterator>
export module vt.Core.ContainerTraits;

namespace vt
{
	export template<typename Container> struct ContainerTraits
	{
		using value_type			 = typename Container::value_type;
		using allocator_type		 = typename Container::allocator_type;
		using size_type				 = typename Container::size_type;
		using difference_type		 = typename Container::difference_type;
		using reference				 = typename Container::reference;
		using const_reference		 = typename Container::const_reference;
		using pointer				 = typename Container::pointer;
		using const_pointer			 = typename Container::const_pointer;
		using iterator				 = typename Container::iterator;
		using const_iterator		 = typename Container::const_iterator;
		using reverse_iterator		 = typename Container::reverse_iterator;
		using const_reverse_iterator = typename Container::const_reverse_iterator;
	};

	export template<typename T> struct ContainerTraits<T[]>
	{
		using value_type			 = T;
		using size_type				 = std::size_t;
		using difference_type		 = std::ptrdiff_t;
		using reference				 = T&;
		using const_reference		 = T const&;
		using pointer				 = T*;
		using const_pointer			 = T const*;
		using iterator				 = T*;
		using const_iterator		 = T const*;
		using reverse_iterator		 = std::reverse_iterator<T*>;
		using const_reverse_iterator = std::reverse_iterator<T const*>;
	};
}
