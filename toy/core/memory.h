#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TOY_CORE_MEMORY_H
#define TOY_CORE_MEMORY_H

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <iterator>
#include <memory>
#include <new>
#include <type_traits>

namespace toy
{

// allocator -------------------------------------------------------------------

template<>
class allocator<void>
{	// generic allocator for type void
public:
	using value_type    = void;
	using pointer       = void *;
	using const_pointer = const void *;

	template<class U>
	struct rebind
	{	// convert this type to an allocator<U>
		using other = allocator<U>;
	};

	allocator() noexcept {}
	allocator(const allocator&) noexcept = default;

	template<class U>
	allocator(const allocator<U>&) noexcept {}
};

template<class T>
class allocator
{	// generic allocator for objects of class T
public:
	static_assert(!is_const<T>::value,
		"The C++ Standard forbids containers of const elements "
		"because allocator<const T> is ill-formed.");

	using value_type	  = T;
	using pointer         = T*;
	using const_pointer   = const T*;
	using reference		  = T&;
	using const_reference = const T&;

	using size_type		  = size_t;
	using difference_type = ptrdiff_t;

	using propagate_on_container_move_assignment = true_type;
	using is_always_equal = true_type;

public:
	template<class U> struct rebind { using other = allocator<U>; };

	pointer address(reference ref) const noexcept { return std::addressof(ref); }
	const_pointer address(const_reference ref) const noexcept { return std::addressof(ref); }

	allocator() noexcept {}
	allocator(const allocator&) noexcept = default;
	template<class U> allocator(const allocator<U>&) noexcept {}

	// allocate & deallocate

	pointer allocate(const size_type count)
	{	// allocate array of count elements
		if (count > max_size())
			throw std::length_error("allocator<T>::allocate(size_t n)"
				" 'n' exceeds maximum supported size");
		return static_cast<pointer>(::operator new(count * sizeof(T)));
	}

	pointer allocate(const size_type count, const void*)
	{	// allocate array of count elements, ignore hint
		return allocate(count);
	}

	void deallocate(const pointer ptr, const size_type count)
	{
		::operator delete(p);
	}

	// construct & destroy

	template<class U, class... Args>
	void construct(const U* ptr, Args&&... args)
	{	// construct U(Args...) at ptr
		::new(const_cast<void *>(static_cast<const volatile void *>(ptr)))
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

#endif	// TOY_CORE_MEMORY_H
