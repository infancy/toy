#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TOY_CORE_VECTOR_H
#define TOY_CORE_VECTOR_H

#include <algorithm>
#include <allocators>
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>

#include "toy/core/initializer_list.h"
#include "toy/core/utility.h"

namespace toy
{

// base class for vector to handle allocator
template <typename T, typename Allocator>
struct VectorBase
{
	using allocator_type  = Allocator;
	using size_type       = size_t;
	using difference_type = ptrdiff_t;
	
	// 'npos' means non-valid position or simply non-position.
	static const size_type npos     = (size_type)-1;      
	// -1 is reserved for 'npos'. It also happens to be slightly beneficial 
	// that kMaxSize is a value less than -1, as it helps us deal with potential 
	// integer wraparound issues.
	static const size_type kMaxSize = (size_type)-2;      

protected:
	T*                                          mpBegin;
	T*                                          mpEnd;
	eastl::compressed_pair<T*, allocator_type>  mCapacityAllocator;

	T*& internalCapacityPtr() EA_NOEXCEPT { return mCapacityAllocator.first(); }
	T* const& internalCapacityPtr() const EA_NOEXCEPT { return mCapacityAllocator.first(); }
	allocator_type&  internalAllocator() EA_NOEXCEPT { return mCapacityAllocator.second(); }
	const allocator_type&  internalAllocator() const EA_NOEXCEPT { return mCapacityAllocator.second(); }

public:
	VectorBase();
	VectorBase(const allocator_type& allocator);
	VectorBase(size_type n, const allocator_type& allocator);

	~VectorBase();

	const allocator_type& get_allocator() const EA_NOEXCEPT;
	allocator_type&       get_allocator() EA_NOEXCEPT;
	void                  set_allocator(const allocator_type& allocator);

protected:
	T*        DoAllocate(size_type n);
	void      DoFree(T* p, size_type n);
	size_type GetNewCapacity(size_type currentCapacity);

};	// VectorBase




}	// namespace toy

#endif	// TOY_CORE_VECTOR_H
