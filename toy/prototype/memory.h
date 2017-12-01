#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TOY_PROTOTYPE_MEMORY_H
#define TOY_PROTOTYPE_MEMORY_H

#include <cstddef>    // for size_t

namespace toy
{

// allocator -------------------------------------------------------------------

template <typename T>
class allocator 
{
public:
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;

	using size_type = size_t;
	using difference_type = ptrdiff_t;

public:
	allocator() noexcept {}
	allocator(const allocator&) noexcept = default;
	template <typename U> allocator(const allocator<U>&) noexcept {}

	// allocate & deallocate

	pointer allocate(const size_type count)
	{	// allocate array of count elements
		if (count > max_size())
			throw std::length_error("allocator<T>::allocate(size_t n)"
				" 'n' exceeds maximum supported size");
		return static_cast<pointer>(::operator new(count * sizeof(T)));
	}

	void deallocate(const pointer ptr, const size_type count)
	{
		::operator delete(p);
	}

	// construct & destroy

	template<class U, class... Args>
	void construct(const U* ptr, Args&&... args)
	{	// construct U(Args...) at ptr
		::new(const_cast<void*>(static_cast<const volatile void*>(ptr)))
			U(std::forward<Args>(args)...);
	}

	template<class U>
	void destroy(const U* ptr)
	{
		ptr->~U();
	}

	size_t max_size() const noexcept
	{
		return (static_cast<size_t>(-1) / sizeof(T));
	}
};

// return that all specializations of this allocator are interchangeable
template <class T1, class T2>
inline bool operator==(const allocator<T1>&, const allocator<T2>&) noexcept
{
	return true;
}

template <class T1, class T2>
inline bool operator!=(const allocator<T1>&, const allocator<T2>&) noexcept
{
	return false;
}

}	// namespace toy	

#endif	// TOY_PROTOTYPE_MEMORY_H
