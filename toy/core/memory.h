#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TOY_CORE_MEMORY_H
#define TOY_CORE_MEMORY_H

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <memory>
#include <new>

#include "toy/core/utility.h"

namespace toy
{

// allocator -------------------------------------------------------------------

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

template<>
class allocator<void>
{	// generic allocator for type void
public:
	using value_type = void;
	using pointer = void *;
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

// Primary template of default_delete, used by unique_ptr
template<typename T>
struct default_delete
{
	constexpr default_delete() noexcept = default;

	template<typename U, typename = typename
		enable_if_t<std::is_convertible<U*, T*>::value>>
	default_delete(const default_delete<U>&) noexcept {}

	void operator()(T* ptr) const
	{
		static_assert(!is_void<T>::value, "can't delete an incomplete type");
		static_assert(0 < sizeof(T),      "can't delete an incomplete type");
		delete ptr;
	}
};

template<class T, class D_noref, class = void>
struct get_deleter_pointer_type
{	// provide fallback
	using type = T*;
};

template<class T, class D_noref>
struct get_deleter_pointer_type<T, D_noref, std::void_t<typename D_noref::pointer>>
{	// get D_noref::pointer
	using type = typename D_noref::pointer;
};

template<class T, class D>
class unique_ptr_base
{	// stores pointer and deleter
public:
	using D_noref = remove_reference_t<D>;
	using pointer = typename get_deleter_pointer_type<T, D_noref>::type;

	template<class Ptr2, class D2>
	unique_ptr_base(Ptr2 ptr, D2&& dt)
		: cpair(std::_One_then_variadic_args_t(), forward<D2>(dt), ptr)
	{	// construct with compatible pointer and deleter
	}

	template<class Ptr2>
	constexpr unique_ptr_base(Ptr2 ptr)
		: cpair(std::_Zero_then_variadic_args_t(), ptr)
	{	// construct with compatible pointer
	}

	D& get_deleter()			 noexcept { return cpair._Get_first(); }
	const D& get_deleter() const noexcept { return cpair._Get_first(); }

	pointer& resource_ptr()				noexcept { return cpair._Get_second(); }
	const pointer& resource_ptr() const noexcept { return cpair._Get_second(); }

	std::_Compressed_pair<D, pointer> cpair;
};

template<class T, class D = default_delete<T>>
class unique_ptr : public unique_ptr_base<T, D>
{	// non-copyable pointer to an object
public:
	// using this_type = unique_ptr<T, D>;
	using base	       = unique_ptr_base<T, D>;
	using pointer      = typename base::pointer;
	using element_type = T;
	using deleter_type = D;

	using base::get_deleter;

	constexpr unique_ptr() noexcept
		: base(pointer())
	{	// default construct
		static_assert(!std::is_pointer<D>::value,
			"unique_ptr constructed with null deleter pointer");
	}

	constexpr unique_ptr(nullptr_t) noexcept
		: base(pointer())
	{	// null pointer construct
		static_assert(!std::is_pointer<D>::value,
			"unique_ptr constructed with null deleter pointer");
	}

	unique_ptr& operator=(nullptr_t) noexcept
	{	// assign a null pointer
		reset();
		return *this;
	}

	explicit unique_ptr(pointer ptr) noexcept
		: base(ptr)
	{	// construct with pointer
		static_assert(!std::is_pointer<D>::value,
			"unique_ptr constructed with null deleter pointer");
	}

	unique_ptr(pointer ptr,
		conditional_t<is_reference<D>::value, D,
		const remove_reference_t<D>&> dt) noexcept
		: base(ptr, dt)
	{	// construct with pointer and (maybe const) deleter&
	}

	unique_ptr(pointer ptr,
		remove_reference_t<D>&& dt) noexcept
		: base(ptr, move(dt))
	{	// construct by moving deleter
		static_assert(!is_reference<D>::value,
			"unique_ptr constructed with reference to rvalue deleter");
	}

	unique_ptr(unique_ptr&& right) noexcept
		: base(right.release(), forward<D>(right.get_deleter()))
	{	// construct by moving right
	}

	template<class T2, class D2, class = enable_if_t<
		!is_array<T2>::value
		&& is_convertible<typename unique_ptr<T2, D2>::pointer, pointer>::value
		&& (is_reference<D>::value ? is_same<D2, D>::value : is_convertible<D2, D>::value)>>
	unique_ptr(unique_ptr<T2, D2>&& right) noexcept
		: base(right.release(), forward<D2>(right.get_deleter()))
	{	// construct by moving right
	}

	template<class T2, class = enable_if_t<
		is_convertible<T2*, T*>::value && is_same<D, default_delete<T>>::value>>
	unique_ptr(std::auto_ptr<T2>&& right) noexcept
		: base(right.release())
	{	// construct by moving right
	}

	template<class T2, class D2, class = enable_if_t<
		!is_array<T2>::value
		&& is_assignable<D&, D2&&>::value
		&& is_convertible<typename unique_ptr<T2, D2>::pointer, pointer>::value>>
	unique_ptr& operator=(unique_ptr<T2, D2>&& right) noexcept
	{	// assign by moving right
		reset(right.release());
		this->get_deleter() = forward<D2>(right.get_deleter());
		return *this;
	}

	unique_ptr& operator=(unique_ptr&& right) noexcept
	{	// assign by moving right
		if (this != std::addressof(right))
		{	// different, do the move
			reset(right.release());
			this->get_deleter() = forward<D>(right.get_deleter());
		}
		return *this;
	}

	void swap(unique_ptr& right) noexcept
	{	// swap elements
		std::_Swap_adl(this->resource_ptr(), right.resource_ptr());
		std::_Swap_adl(this->get_deleter(), right.get_deleter());
	}

	~unique_ptr() noexcept
	{	// destroy the object
		if (get() != pointer())
			this->get_deleter()(get());
	}

	add_lvalue_reference_t<T> operator*() const
	{	// return reference to object
		return *get();
	}

	pointer operator->() const noexcept
	{	// return pointer to class object
		return this->resource_ptr();
	}

	pointer get() const noexcept
	{	// return pointer to object
		return this->resource_ptr();
	}

	explicit operator bool() const noexcept
	{	// test for non-null pointer
		return get() != pointer();
	}

	pointer release() noexcept
	{	// yield ownership of pointer
		pointer ptr = get();
		this->resource_ptr() = pointer();
		return ptr;
	}

	void reset(pointer ptr = {}) noexcept
	{	// establish new pointer
		pointer old = get();
		this->resource_ptr() = ptr;
		if (old != pointer())
			this->get_deleter()(old);
	}

	unique_ptr(const unique_ptr&) = delete;
	unique_ptr& operator=(const unique_ptr&) = delete;
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
