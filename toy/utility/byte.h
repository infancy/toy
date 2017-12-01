#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TOY_UTILITY_BYTE_H
#define TOY_UTILITY_BYTE_H

#include <cstdint>

namespace toy
{

// bit & byte ------------------------------------------------------------------

// 判断字节序

inline bool is_little_endian_order()
{
	int endian = 0x12;
	return *((char*)&endian) == 0x12;
}

using byte = uint8_t;	// 如何保证局部性
using block = uint32_t;

// byte转block（左低右高）
inline void byte_to_block(const byte* input, block* output, size_t length)
{
	for (size_t i = 0, j = 0; j < length; ++i, j += 4)
	{
		output[i] = ((static_cast<block>(input[j]))
			| ((static_cast<block>(input[j + 1])) << 8)
			| ((static_cast<block>(input[j + 2])) << 16)
			| ((static_cast<block>(input[j + 3])) << 24));
	}
}

inline void block_to_byte(const block* input, byte* output, size_t length)
{
	for (size_t i = 0, j = 0; j < length; ++i, j += 4)
	{
		output[j] = static_cast<byte>(input[i] & 0xff);
		output[j + 1] = static_cast<byte>((input[i] >> 8) & 0xff);
		output[j + 2] = static_cast<byte>((input[i] >> 16) & 0xff);
		output[j + 3] = static_cast<byte>((input[i] >> 24) & 0xff);
	}
}

}	// namespace toy	

#endif	// TOY_UTILITY_BYTE_H
