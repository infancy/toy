#if defined(_MSC_VER)
#pragma once
#endif

#include <type_traits>

#ifndef TOY_CORE_TYPE_TRAITS_H
#define TOY_CORE_TYPE_TRAITS_H

namespace toy
{

// C++17 Standard, section 23.15.

// helper class ----------------------------------------------------------------
// convenient template for integral constant types

template<class T, T v>
struct integral_constant
{
	static constexpr T value = v;

	using value_type = T;
	using type = integral_constant;

	constexpr operator value_type() const noexcept { return value; }

	constexpr value_type operator()() const noexcept { return value; }
};

template <bool b>
using bool_constant = integral_constant<bool, b>;

using true_type  = bool_constant<true>;
using false_type = bool_constant<false>;

// enable_if -------------------------------------------------------------------
// 若 b == true 则得到 T，否则 ::type 不存在（从而在大多数情况下导致代入失败）

template<bool b, class T = void> 
struct enable_if {};

template<class T> 
struct enable_if<true, T> { using type = T; };

template<bool b, class T = void>
using enable_if_t = typename enable_if<b, T>::type;

// conditional -----------------------------------------------------------------
// 若 b == true 则得到类型 T， 否则得到 F

template<bool b, class T, class F> 
struct conditional { using type = F; };

template<class T, class F> 
struct conditional<true, T, F> { using type = T; };

template<bool b, class T, class F>
using conditional_t = typename conditional<b, T, F>::type;

// is_same ---------------------------------------------------------------------
// determine whether T and U are the same type

template<class T, class U> 
struct is_same : false_type {};

template<class T> 
struct is_same<T, T> : true_type {};

template<class T, class U>
constexpr bool is_same_v = is_same<T, U>::value;


// -----------------------------------------------------------------------------
// const-volatile modifications
// -----------------------------------------------------------------------------


// remove_const ----------------------------------------------------------------
// remove top level const qualifier

template<class T>
struct remove_const { using type = T; };

template<class T>
struct remove_const<const T> { using type = T; };

template<class T>
using remove_const_t = typename remove_const<T>::type;

// remove_volatile -------------------------------------------------------------
// remove top level volatile qualifier

template<class T>
struct remove_volatile { using type = T; };

template<class T>
struct remove_volatile<volatile T> { using type = T; };

template<class T>
using remove_volatile_t = typename remove_volatile<T>::type;

// remove_cv -------------------------------------------------------------------
// remove top level const and volatile qualifiers

template<class T>
struct remove_cv { using type = T; };

template<class T>
struct remove_cv<const T> { using type = T; };

template<class T>
struct remove_cv<volatile T> { using type = T; };

template<class T>
struct remove_cv<const volatile T> { using type = T; };

template<class T>
using remove_cv_t = typename remove_cv<T>::type;

// add_const -------------------------------------------------------------------
// add top level const qualifier

template<class T>
struct add_const { using type = const T; };

template<class T>
using add_const_t = typename add_const<T>::type;

// add_volatile ----------------------------------------------------------------
// add top level volatile qualifier

template<class T>
struct add_volatile { using type = volatile T; };

template<class T>
using add_volatile_t = typename add_volatile<T>::type;

// add_cv ----------------------------------------------------------------------
// add top level const and volatile qualifiers

template<class T>
struct add_cv { using type = const volatile T; };

template<class T>
using add_cv_t = typename add_cv<T>::type;


// -----------------------------------------------------------------------------
// primary type categories
// -----------------------------------------------------------------------------


// is_void ---------------------------------------------------------------------
// determine whether T is void

template <class T> 
struct _is_void : false_type {};

template <>          
struct _is_void<void> : true_type {};

template <class T> 
struct is_void : _is_void<remove_cv_t<T>> {};

template <class T> 
constexpr bool is_void_v = is_void<T>::value;

// is_null_pointer -------------------------------------------------------------
// determine whether T is nullptr_t

template<class T>
struct is_null_pointer
	: bool_constant<is_same<remove_cv_t<T>, nullptr_t>::value> {};

template<class T>
constexpr bool is_null_pointer_v = is_null_pointer<T>::value;

// is_integral -----------------------------------------------------------------
// determine whether T is integral

template<class T> struct _is_integral : false_type {};	// default is false

template<> struct _is_integral<bool>		   : true_type {};
template<> struct _is_integral<char>		   : true_type {};
template<> struct _is_integral<short>		   : true_type {};
template<> struct _is_integral<int>			   : true_type {};
template<> struct _is_integral<long>		   : true_type {};
template<> struct _is_integral<long long>	   : true_type {};

template<> struct _is_integral<signed char>    : true_type {};
template<> struct _is_integral<unsigned char>  : true_type {};
template<> struct _is_integral<unsigned short> : true_type {};
template<> struct _is_integral<unsigned int>   : true_type {};
template<> struct _is_integral<unsigned long>  : true_type {};
template<> struct _is_integral<unsigned long long> : true_type {};

template<> struct _is_integral<wchar_t>  : true_type {};
template<> struct _is_integral<char16_t> : true_type {};
template<> struct _is_integral<char32_t> : true_type {};

template<class T>
struct is_integral : _is_integral<remove_cv_t<T>>::type {};

template<class T>
constexpr bool is_integral_v = is_integral<T>::value;

// is_floating_point -----------------------------------------------------------
// determine whether T is floating point

template<class T> struct _is_floating_point : false_type {};	// default is false

template<> struct _is_floating_point<float>		  : true_type {};
template<> struct _is_floating_point<double>	  : true_type {};
template<> struct _is_floating_point<long double> : true_type {};

template<class T>
struct is_floating_point : _is_floating_point<remove_cv_t<T>>::type {};

template<class T>
constexpr bool is_floating_point_v = is_floating_point<T>::value;

// is_lvalue_reference ---------------------------------------------------------
// determine whether T is an lvalue reference

template<class T>
struct is_lvalue_reference : false_type {};

template<class T>
struct is_lvalue_reference<T&> : true_type {};

template<class T>
constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

// is_rvalue_reference ---------------------------------------------------------
// determine whether T is an rvalue reference

template<class T>
struct is_rvalue_reference : false_type {};

template<class T>
struct is_rvalue_reference<T&&> : true_type {};

template<class T>
constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

// -----------------------------------------------------------------------------
// composite type categories
// -----------------------------------------------------------------------------


// is_reference ----------------------------------------------------------------
// determine whether T is a reference

template<class T>
struct is_reference : bool_constant<
	is_lvalue_reference<T>::value || is_rvalue_reference<T>::value> {};

template<class T>
constexpr bool is_reference_v = is_reference<T>::value;

// is_arithmetic ---------------------------------------------------------------
// determine whether T is an arithmetic type

template<class T>
struct is_arithmetic : bool_constant<
	is_integral<T>::value || is_floating_point<T>::value> {};

template<class T>
constexpr bool is_arithmetic_v = is_arithmetic<T>::value;


// -----------------------------------------------------------------------------
// type properties
// -----------------------------------------------------------------------------


// is_const --------------------------------------------------------------------

template <class T>
struct is_const : false_type {};

template <class T>
struct is_const<const T> : true_type {};

template <class T>
constexpr bool is_const_v = is_const<T>::value;

// is_volatile -----------------------------------------------------------------

template <class T>
struct is_volatile : false_type {};

template <class T>
struct is_volatile<volatile T> : true_type {};

template <class T>
constexpr bool is_volatile_v = is_volatile<T>::value;


// -----------------------------------------------------------------------------
// reference modifications
// -----------------------------------------------------------------------------


// remove_reference ------------------------------------------------------------

template <class T> 
struct remove_reference { using type = T; };

template <class T> 
struct remove_reference<T&> { using type = T; };

template <class T>
struct remove_reference<T&&> { using type = T; };

template <class T> 
using remove_reference_t = typename remove_reference<T>::type;

// _add_reference --------------------------------------------------------------

template<class T, class = void>
struct _add_reference
{
	using lvalue = T;
	using rvalue = T;
};

template<class T>
struct _add_reference<T, std::void_t<T&>>
{
	using lvalue = T&;
	using rvalue = T&&;
};

// add_lvalue_reference --------------------------------------------------------

template<class T>
struct add_lvalue_reference
{
	using type = typename _add_reference<T>::lvalue;
};

template<class T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

// add_rvalue_reference --------------------------------------------------------

template<class T>
struct add_rvalue_reference
{
	using type = typename _add_reference<T>::rvalue;
};

template<class T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;


// -----------------------------------------------------------------------------
// forward & move
// -----------------------------------------------------------------------------


// forward ---------------------------------------------------------------------

template<class T>
constexpr T&& 
forward(remove_reference_t<T>& t) noexcept
{	// forward an lvalue as either an lvalue or an rvalue
	return static_cast<T&&>(t);
}

template<class T>
constexpr T&& 
forward(remove_reference_t<T>&& t) noexcept
{	// forward an rvalue as an rvalue
	static_assert(!is_lvalue_reference<T>::value, "bad forward call");
	return static_cast<T&&>(t);
}

// move ------------------------------------------------------------------------
// forward t as movable

template <class T>
constexpr remove_reference_t<T>&& 
move(T&& t) noexcept
{
	return static_cast<remove_reference_t<T>&&>(t);
}

}	// namespace toy

#endif	// TOY_CORE_TYPE_TRAITS_H
