#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TOY_CORE_STDDEF_H
#define TOY_CORE_STDDEF_H

namespace toy
{

class nullptr_t
{
public:
	template<class T>
	operator T*() const
	{
		return 0;
	}

	template<class C, class T>
	operator T C::*() const
	{
		return 0;
	}

	// bool b = (bool_)nullptr;
	// typedef void* (nullptr_t::*bool_)() const;
	// operator bool_() const { return false; }

	void operator&() const = delete;
};

const nullptr_t toy_nullptr = {};
// using toy::nullptr;

}	// namespace toy	

#endif	// TOY_CORE_STDDEF_H