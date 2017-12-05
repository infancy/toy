#if defined(_MSC_VER)
#pragma once
#endif

#include <type_traits>
#include <utility>

#include "toy/core/type_traits.h"

#ifndef TOY_STD_UTILITY_H
#define TOY_STD_UTILITY_H

namespace toy
{

namespace
{

// C++17 Standard, section 23.2~23.4.

// swap ------------------------------------------------------------------------

template<class T> 
inline void swap(T& left, T& right) 
	noexcept(std::is_nothrow_move_constructible<T>::value && std::is_nothrow_move_assignable<T>::value)
{
	T tmp = toy::move(left);
	left  = toy::move(right);
	right = toy::move(tmp);
}

template<class T, size_t N>
inline void swap(T(&left)[N], T(&right)[N])
noexcept(std::_Is_nothrow_swappable<T>::value)		// C++17:is_nothrow_swappable_v<T>
{
	for (size_t i = 0; i < N; ++i)
		swap(left[i], right[i]);		// swap_iter()
}

// pair ------------------------------------------------------------------------

template<class T1, class T2>
struct pair
{
	using first_type  = T1;
	using second_type = T2;

	T1 first;	// the first stored value
	T2 second;	// the second stored value

	template<class U1 = T1, class U2 = T2, class =  enable_if_t<
		std::is_default_constructible<U1>::value && std::is_default_constructible<U2>::value>>
	constexpr pair()
		: first(), second()
	{	// default construct
	}

	template<class U1 = T1, class U2 = T2, class = enable_if_t<
		std::is_copy_constructible<U1>::value && std::is_copy_constructible<U2>::value>,
		enable_if_t<std::is_convertible<const U1&, U1>::value && std::is_convertible<const U2&, U2>::value, int> = 0>
	constexpr pair(const T1& a, const T2& b)
		: first(a), second(b)
	{	// construct from specified values
	}

	template<class U1 = T1, class U2 = T2, class = enable_if_t<
		std::is_copy_constructible<U1>::value && std::is_copy_constructible<U2>::value>,
		enable_if_t<!std::is_convertible<const U1&, U1>::value || !std::is_convertible<const U2&, U2>::value, int> = 0>
	explicit constexpr pair(const T1& a, const T2& b)
		: first(a), second(b)
	{	// construct from specified values
	}

	pair(const pair&) = default;
	pair(pair&&) = default;

	template<class U1, class U2, class = enable_if_t<
		std::is_constructible<T1, const U1&>::value && std::is_constructible<T2, const U2&>::value>,
		enable_if_t<std::is_convertible<const U1&, T1>::value && std::is_convertible<const U2&, T2>::value, int> = 0>
	constexpr pair(const pair<U1, U2>& right)
		: first(right.first), second(right.second)
	{	// construct from compatible pair
	}

	template<class U1, class U2, class = enable_if_t<
		std::is_constructible<T1, const U1&>::value && std::is_constructible<T2, const U2&>::value>,
		enable_if_t<!std::is_convertible<const U1&, T1>::value || !std::is_convertible<const U2&, T2>::value, int> = 0>
	constexpr explicit pair(const pair<U1, U2>& right)
		: first(right.first), second(right.second)
	{	// construct from compatible pair
	}

	template<class U1, class U2>
	pair& operator=(const pair<U1, U2>& right)
	{	// assign from compatible pair
		first = right.first;
		second = right.second;
		return *this;
	}

	template<class Tuple1, class Tuple2,
		size_t... Indexes1, size_t... Indexes2> 
	inline pair(Tuple1& a, Tuple2& b,
		std::index_sequence<Indexes1...>,
		std::index_sequence<Indexes2...>);

	template<class... Types1, class... Types2> 
	inline pair(std::piecewise_construct_t,
			std::tuple<Types1...> a,
			std::tuple<Types2...> b);

	template<class U1, class U2, class = enable_if_t<
		std::is_constructible<T1, U1>::value && std::is_constructible<T2, U2>::value>,
		enable_if_t<std::is_convertible<U1, T1>::value && std::is_convertible<U2, T2>::value, int> = 0>
	constexpr pair(U1&& a, U2&& b) 
		noexcept(std::is_nothrow_constructible<T1, U1>::value && std::is_nothrow_constructible<T2, U2>::value)
		: first(toy::forward<U1>(a)), second(toy::forward<U2>(b))
	{	// construct from moved values
	}

	template<class U1, class U2, class = enable_if_t<
		std::is_constructible<T1, U1>::value && std::is_constructible<T2, U2>::value>,
		enable_if_t<!std::is_convertible<U1, T1>::value || !std::is_convertible<U2, T2>::value, int> = 0>
	constexpr explicit pair(U1&& a, U2&& b)
		noexcept(std::is_nothrow_constructible<T1, U1>::value && std::is_nothrow_constructible<T2, U2>::value)
		: first(toy::forward<U1>(a)), second(toy::forward<U2>(b))
	{	// construct from moved values
	}

	template<class U1, class U2, class = enable_if_t<
		std::is_constructible<T1, U1>::value && std::is_constructible<T2, U2>::value>,
		enable_if_t<std::is_convertible<U1, T1>::value && std::is_convertible<U2, T2>::value, int> = 0>
	constexpr pair(pair<U1, U2>&& right)
		noexcept(std::is_nothrow_constructible<T1, U1>::value && std::is_nothrow_constructible<T2, U2>::value)
		: first(toy::forward<U1>(right.first)), second(toy::forward<U2>(right.second))
	{	// construct from moved compatible pair
	}

	template<class U1, class U2, class = enable_if_t<
		std::is_constructible<T1, U1>::value && std::is_constructible<T2, U2>::value>,
		enable_if_t<!std::is_convertible<U1, T1>::value || !std::is_convertible<U2, T2>::value, int> = 0>
	constexpr explicit pair(pair<U1, U2>&& right)
		noexcept(std::is_nothrow_constructible<T1, U1>::value && std::is_nothrow_constructible<T2, U2>::value)
		: first(toy::forward<U1>(right.first)), second(toy::forward<U2>(right.second))
	{	// construct from moved compatible pair
	}

	template<class U1, class U2>
	pair& operator=(pair<U1, U2>&& right)
	{	// assign from moved compatible pair
		first = toy::forward<U1>(right.first);
		second = toy::forward<U2>(right.second);
		return *this;
	}

	pair& operator=(pair&& right)
		noexcept(std::is_nothrow_move_assignable<T1>::value && std::is_nothrow_move_assignable<T2>::value)
	{	// assign from moved pair
		first = toy::forward<T1>(right.first);
		second = toy::forward<T2>(right.second);
		return *this;
	}

	pair& operator=(const pair& right)
	{	// assign from copied pair
		first = right.first;
		second = right.second;
		return *this;
	}

	void swap(pair& right)
		noexcept(std::_Is_nothrow_swappable<T1>::value && std::_Is_nothrow_swappable<T2>::value)
	{	// exchange contents with right
		if (this != std::addressof(right))
		{	// different, worth swapping
			toy::swap(first, right.first);
			toy::swap(second, right.second);
		}
	}
};

// pair functions --------------------------------------------------------------

template<class T1, class T2,
	class = enable_if_t<std::_Is_swappable<T1>::value && std::_Is_swappable<T2>::value>>
inline void swap(pair<T1, T2>& left, pair<T1, T2>& right)
	noexcept(noexcept(left.swap(right)))
{	// swap left and right pairs
	left.swap(right);
}

template<class T1, class T2>
constexpr bool operator==(const pair<T1, T2>& left, const pair<T1, T2>& right)
{	// test for pair equality
	return (left.first == right.first && left.second == right.second);
}

template<class T1, class T2>
constexpr bool operator!=(const pair<T1, T2>& left, const pair<T1, T2>& right)
{	// test for pair inequality
	return (!(left == right));
}

template<class T1, class T2>
constexpr bool operator<(const pair<T1, T2>& left, const pair<T1, T2>& right)
{	// test if left < right for pairs
	return (left.first < right.first ||
		(!(right.first < left.first) && left.second < right.second));
}

template<class T1, class T2>
constexpr bool operator>(const pair<T1, T2>& left, const pair<T1, T2>& right)
{	// test if left > right for pairs
	return (right < left);
}

template<class T1, class T2>
constexpr bool operator<=(const pair<T1, T2>& left, const pair<T1, T2>& right)
{	// test if left <= right for pairs
	return (!(right < left));
}

template<class T1, class T2>
constexpr bool operator>=(const pair<T1, T2>& left, const pair<T1, T2>& right)
{	// test if left >= right for pairs
	return (!(left < right));
}

//  make_pair ------------------------------------------------------------------

template<class T1, class T2>
constexpr pair<typename std::_Unrefwrap<T1>::type, typename std::_Unrefwrap<T2>::type>
make_pair(T1&& a, T2&& b)
{	// return pair composed from arguments
	using Pair = pair<typename std::_Unrefwrap<T1>::type, typename std::_Unrefwrap<T2>::type>;
	return (Pair(toy::forward<T1>(a), toy::forward<T2>(b)));
}

}	// namespace

}	// namespace toy

#endif	// TOY_STD_UTILITY_H
