#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TOY_SECURE_HASH_H
#define TOY_SECURE_HASH_H

#include <fstream>
#include <string>
#include <toy/utility/byte.h>

namespace toy
{

using byte = uint8_t;	// 如何保证局部性?
using block = uint32_t;

// message digest 5th ----------------------------------------------------------

// references
// https://zh.wikipedia.org/wiki/MD5
// https://www.cnblogs.com/fullsail/archive/2013/02/22/2921505.html

class MD5
{
public:
	void encode(const std::string& message);
	void encode(const std::ifstream& file);

	const uint8_t* to_hash();
	// e.g., ":" -> "FF:FF:FF:FF"
	std::string to_hex_string(const char* separate_format = ":");
	// std::string to_randomart_image();

private:
	void encode(const byte* message, uint64_t length);
	void process(const block group[16], block hash[4]);

private:
	// MD5 默认处理小端序的数据，当处理大端序时需要进行转换
	// bool is_little_end{ is_little_endian_order() };

	const block 
		S11 = 7, S12 = 12, S13 = 17, S14 = 22,
		S21 = 5, S22 = 9,  S23 = 14, S24 = 20,
		S31 = 4, S32 = 11, S33 = 16, S34 = 23,
		S41 = 6, S42 = 10, S43 = 15, S44 = 21;

	byte  result[16]{};
};

// Secure Hash Algorithm 2nd ---------------------------------------------------

// SHA-256




}	// namespace toy	

#endif	// TOY_SECURE_HASH_H
