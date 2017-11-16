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

// ������ֱ�Ӵ��� noncopyable ���󣬵����Դ����̳��� noncopyable ������
// �������ඨ���Լ��� copy ����͸�ֵ�������������� copy/move
class noncopyable
{
protected:
	noncopyable() = default;
	~noncopyable() = default;

	noncopyable(const noncopyable&) = delete;
	noncopyable& operator=(const noncopyable&) = delete;

	// �� C++11 ��׼�У��û����� �������� �����������Ĭ������ move funciton
	//noncopyable(const noncopyable&&) = delete;
	//noncopyable& operator=(const noncopyable&&) = delete;
};


}	//namespace toy

#endif //TOY_UTI_TOY_H
