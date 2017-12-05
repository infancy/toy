#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TOY_CORE_MEMORY_H
#define TOY_CORE_MEMORY_H

#include <cstddef>    // for size_t
#include <type_traits>
#include <utility>

#include "toy/core/utility.h"

namespace toy
{

// allocator -------------------------------------------------------------------

template <typename T>
class allocator 
{
public:
	using value_type	  = T;
	using pointer		  = T*;
	using const_pointer   = const T*;
	using reference		  = T&;
	using const_reference = const T&;

	using size_type		  = size_t;
	using difference_type = ptrdiff_t;

public:
	// rebind template is an archaic alias. It should have been: 
	// template<class U> using other = allocator<U>;
	// However, allocator was defined before such aliases were supported by C++. 
	// It is provided to allow an allocator to allocate objects of arbitrary type.Consider:
	// using Link_alloc = typename A::template rebind<Link>::other;
	template<class U> struct rebind { using other = allocator<U>; };

	// constructors
	allocator() noexcept {}
	allocator(const allocator&) noexcept = default;
	template <typename U> allocator(const allocator<U>&) noexcept {}
	~allocator() {}

	//
	pointer address(reference ref) const noexcept { return std::addressof(ref); }
	const_pointer address(const_reference ref) const noexcept { return std::addressof(ref); }

	// allocate & deallocate
	pointer allocate(size_t count)
	{	// allocate array of count elements
		if (count > max_size())
			throw std::length_error("allocator<T>::allocate(size_t n)"
				" 'n' exceeds maximum supported size");
		return static_cast<pointer>(::operator new(count * sizeof(T)));
	}

	void deallocate(pointer ptr, size_t count)
	{
		::operator delete(ptr);
	}

	// construct & destroy
	template<class U, class... Args>
	void construct(const U* ptr, Args&&... args)
	{	// new((void*)U) U{args}, construct U(Args...) at ptr
		::new(const_cast<void*>(static_cast<const volatile void*>(ptr)))
			U{std::forward<Args>(args)...};
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

template<>
class allocator<void>
{	// generic allocator for type void
public:
	using value_type	= void;
	using pointer		= void*;
	using const_pointer = const void*;

	template<class U> struct rebind { using other = allocator<U>; };

	allocator() noexcept {}
	allocator(const allocator&) noexcept = default;
	template<class U> allocator(const allocator<U>&) noexcept {}
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

// unique_ptr ------------------------------------------------------------------

template<typename T>
struct default_delete
{
	void operator()(T* ptr) const
	{
		static_assert(!is_void<T>::value, "can't delete an incomplete type");
		static_assert(0 < sizeof(T),	  "can't delete an incomplete type");
		delete ptr;
	}
};

template<typename T, class D = default_delete<T>>
class unique_ptr
{
public:
	using pointer = T*;
	using element_type = T;
	using deleter_type = D;

public:
	// constructors
	constexpr unique_ptr() noexcept {}
	constexpr unique_ptr(nullptr_t) noexcept {}
	explicit unique_ptr(pointer p) noexcept : ptr{p} {}

	// unique_ptr(pointer p, D& d) noexcept
	unique_ptr(pointer p, conditional_t<is_reference<D>::value, D, const remove_reference_t<D>&> d) noexcept
		: ptr{ p }, deleter{ d } {}
	// unique_ptr(pointer p, D&& d) noexcept
	unique_ptr(pointer p, remove_reference_t<D>&& d) noexcept
		: ptr{ p }, deleter{ std::move(d) } {}

	unique_ptr(unique_ptr&& u)
		: ptr{ u.release() }, deleter{ std::forward<D>(u.deleter) } {}

	template <class U, class E>
	unique_ptr(unique_ptr<U, E>&& u) noexcept
		: ptr{ u.release() }, deleter{ std::forward<E>(u.deleter) } {}


	// destructor
	~unique_ptr() { reset(); }

	// assignment
	unique_ptr& operator=(unique_ptr&& u)
	{
		if (this != std::addressof(u))
		{
			reset(u.release());
			get_deleter() = std::forward<D>(u.get_deleter());
		}
		return *this;
	}

	template <class U, class E> 
	unique_ptr& operator=(unique_ptr<U, E>&& u) noexcept
	{
		reset(u.release());
		get_deleter() = std::forward<E>(u.get_deleter());
		return *this;
	}

	unique_ptr& operator=(nullptr_t) noexcept
	{
		reset();
		return *this;
	}

	// observers
	std::add_lvalue_reference_t<T> operator*() const { return *get(); }		// return reference to object
	pointer operator->() const noexcept { return get(); }	// (uptr.operator->())->member
	pointer get() const noexcept { return ptr; }

	deleter_type& get_deleter() noexcept { return deleter; }
	const deleter_type& get_deleter() const noexcept { return deleter; }
	explicit operator bool() const noexcept { return get() != pointer(); }

	// modifiers
	pointer release() noexcept
	{
		pointer p = get();
		ptr = pointer();
		return p;
	}

	void reset(pointer p = {}) noexcept
	{
		if (p != ptr)
		{
			get_deleter()(ptr);
			ptr = p;
		}
	}

	void swap(unique_ptr& u) noexcept
	{
		std::swap(ptr, u.ptr);
		std::swap(deleter, u.deleter);
	}

	// disable copy from lvalue
	unique_ptr(const unique_ptr&) = delete;
	unique_ptr& operator=(const unique_ptr&) = delete;

private:
	pointer ptr;
	deleter_type deleter;
};

template<class T, class D, class = enable_if_t<std::_Is_swappable<D>::value>>
void swap(unique_ptr<T, D>& left, unique_ptr<T, D>& right) noexcept
{
	left.swap(right);
}

template<class T1, class D1, class T2, class D2>
bool operator==(const unique_ptr<T1, D1>& left, const unique_ptr<T2, D2>& right)
{	// test if unique_ptr left equals right
	return left.get() == right.get();
}

template<class T1, class D1, class T2, class D2>
bool operator<(const unique_ptr<T1, D1>& left, const unique_ptr<T2, D2>& right)
{	// test if unique_ptr left precedes right
	using ptr1 = typename unique_ptr<T1, D1>::pointer;
	using ptr2 = typename unique_ptr<T2, D2>::pointer;
	using common = std::common_type_t<ptr1, ptr2>;

	return std::less<common>()(left.get(), right.get());
}

// make_unique -----------------------------------------------------------------

template<class T, class... Types, class = enable_if_t<!std::is_array<T>::value>>
inline unique_ptr<T> make_unique(Types&&... args)
{
	return unique_ptr<T>(new T(forward<Types>(args)...));
}

template<class T, class... Types, class = enable_if_t<std::extent<T>::value != 0>>
void make_unique(Types&&...) = delete;

// shared_ptr ------------------------------------------------------------------










// weak_ptr --------------------------------------------------------------------

}	// namespace toy	

#endif	// TOY_CORE_MEMORY_H
