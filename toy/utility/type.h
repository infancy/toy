#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TOY_UTILITY_TYPE_H
#define TOY_UTILITY_TYPE_H

namespace toy
{

template<typename T, size_t N>
using Array = T[N];

// Array<int, 10> a;

}	// namespace toy	

#endif	// TOY_UTILITY_TYPE_H