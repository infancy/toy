#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef TOY_MATH_T_T_H
#define TOY_MATH_T_T_H

#include"toy.h"

namespace toy
{

class stream
{

/*
// 取出 str 中有内容的部分
std::string trim(const std::string& str,
const std::string& whitespace = " \t")
{
const auto strBegin = str.find_first_not_of(whitespace);
if (strBegin == std::string::npos)
return ""; // no content

const auto strEnd = str.find_last_not_of(whitespace);
const auto strRange = strEnd - strBegin + 1;

return str.substr(strBegin, strRange);
}
*/
};

}	//namespace toy

#endif //TOY_MATH_T_T_H
