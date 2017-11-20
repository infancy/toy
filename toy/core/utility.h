#if defined(_MSC_VER)
#pragma once
#endif

#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

#include "toy/core/initializer_list.h"
#include "toy/core/type_traits.h"

#ifndef TOY_CORE_UTILITY_H
#define TOY_CORE_UTILITY_H

namespace toy
{

// C++17 Standard, section 23.2~23.4.

// swap ------------------------------------------------------------------------

template<class T, size_t N> 
inline void swap(T (&left)[N], T (&right)[N])
	noexcept(std::_Is_nothrow_swappable<T>::value)
{
	T* from = left;
	T* end  = left + N;
	T* dest = right;

	for (; from != end; ++from, ++dest)
		swap(*from, *dest);		// swap_iter()
}

template<class T> 
inline void swap(T& left, T& right) 
	noexcept(std::is_nothrow_move_constructible<T>::value && std::is_nothrow_move_assignable<T>::value)
{
	T tmp = toy::move(left);
	left  = toy::move(right);
	right = toy::move(tmp);
}

// pair ------------------------------------------------------------------------

}	// namespace toy

#endif	// TOY_CORE_UTILITY_H
