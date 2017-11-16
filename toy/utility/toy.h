#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef TOY_UTI_TOY_H
#define TOY_UTI_TOY_H

// Global Include Files
#include <unordered_map>
#include <type_traits>
#include <algorithm>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <cinttypes>
#include <cmath>
#include <cassert>
#include <cstring>

#include <glog/logging.h>

namespace toy
{

// 不允许直接创建 noncopyable 对象，但可以创建继承自 noncopyable 的子类
// 除非子类定义自己的 copy 构造和赋值函数，否则不允许 copy/move
class noncopyable
{
protected:
	noncopyable() = default;
	~noncopyable() = default;

	noncopyable(const noncopyable&) = delete;
	noncopyable& operator=(const noncopyable&) = delete;

	// 在 C++11 标准中，用户定义 析构函数 后编译器不会默认生成 move funciton
	//noncopyable(const noncopyable&&) = delete;
	//noncopyable& operator=(const noncopyable&&) = delete;
};


}	//namespace toy

#endif //TOY_UTI_TOY_H
