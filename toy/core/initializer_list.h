#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TOY_CORE_INITIALIZER_LIST_H
#define TOY_CORE_INITIALIZER_LIST_H

#include <cstddef>	// for size_t 

namespace toy
{

// see the C++11/14 Standard, section 18.9.
// see the C++17 Standard, section 21.9.

template<typename E>
class initializer_list
{
public:
	using value_type      = E;
	using reference       = const E&;
	using cosnt_reference = const E&;
	using size_type       = size_t;

	using iterator        = const E*;
	using const_iterator  = const E*;

public:
	constexpr initializer_list() noexcept
		: first(nullptr), last(nullptr) 
	{
	}

	constexpr initializer_list(E* first_arg, E* last_arg) noexcept
		: first(first_arg), last(last_arg)
	{
	}

	constexpr size_t size() const noexcept
	{
		return static_cast<size_t>(last - first);
	}

	constexpr const E* begin() const noexcept
	{
		return first;
	}

	constexpr const E* end() const noexcept
	{
		return last;
	}

private:
	E* first;
	E* last;
};

template<class E> 
constexpr const E* begin(initializer_list<E> il) noexcept
{
	return il.begin();
}

template<class E> 
constexpr const E* end(initializer_list<E> il) noexcept
{
	return il.end();
}

}	// namespace toy

#endif // TOY_CORE_INITIALIZER_LIST_H