#include "toy/secure/hash.h"

#include <sstream>
using namespace std;

namespace toy
{

// message digest 5th ----------------------------------------------------------

namespace
{	// support functions

// 循环左移
uint32_t left_rotate(uint32_t num, uint32_t n)
{
	return (num << n) | (num >> (32 - n));
}

block F(block x, block y, block z) { return (x & y) | ((~x) & z); }
block G(block x, block y, block z) { return (x & z) | (y & (~z)); }
block H(block x, block y, block z) { return x ^ y ^ z; }
block I(block x, block y, block z) { return y ^ (x | (~z)); }

void FF(block& a, block b, block c, block d, block Mj, block s, block Ti)
{
	block temp = a + F(b, c, d) + Mj + Ti;
	a = b + left_rotate(temp, s);
}

void GG(block& a, block b, block c, block d, block Mj, block s, block Ti)
{
	block temp = a + G(b, c, d) + Mj + Ti;
	a = b + left_rotate(temp, s);
}

void HH(block& a, block b, block c, block d, block Mj, block s, block Ti)
{
	block temp = a + H(b, c, d) + Mj + Ti;
	a = b + left_rotate(temp, s);
}

void II(block& a, block b, block c, block d, block Mj, block s, block Ti)
{
	block temp = a + I(b, c, d) + Mj + Ti;
	a = b + left_rotate(temp, s);
}

}	// namespace

void MD5::encode(const string& message)
{
	encode((const byte*)(message.c_str()), message.length());
}

void MD5::encode(const ifstream& file)
{
	if (!file)
		throw std::ios_base::failure("invalid file");

	stringstream ss; ss << file.rdbuf();
	string message{ ss.str() };

	encode((const byte*)(message.c_str()), message.length());
}

const uint8_t* MD5::to_hash()
{
	return result;
}

string MD5::to_hex_string(const char* separate_format)
{
	static const string byte_to_hex{ "0123456789ABCDEF" };

	string str;

	for (auto byte : result)
	{
		auto tmp = static_cast<uint32_t>(byte);
		auto a = tmp / 16U;
		auto b = tmp % 16U;
		str.append(1, byte_to_hex[a]);
		str.append(1, byte_to_hex[b]);
		str += separate_format;
	}
	
	str.pop_back();
	return str;
}

void MD5::encode(const byte* message, uint64_t length)
{
	block hash[4]{ 0x67452301U, 0xefcdab89U, 0x98badcfeU, 0x10325476U };

	// 一、处理数据开头能组成完整 64*bytes 的部分

	uint64_t unprocess = length;

	while (unprocess >= 64)
	{
		process(reinterpret_cast<const block*>(message), hash);
		message += 64;
		unprocess -= 64;
	}

	// 二、处理数据末尾的部分，我们要拼出最后一个（或两个）要处理的 block

	// 1.填入剩余数据

	block tail[16]{};
	memcpy(tail, message, unprocess);

	// 2.在数据末尾插入 0x80

	// 例如需要在 0x12340000 中 插入 0x80
	// 则 00 00 34 12 -> 00 80 34 12

	uint32_t index = unprocess / 4;		// 在哪个 block 中
	uint32_t shift = (unprocess & 3) * 8;	// 需要在这个 block 中移动几位

	tail[index] ^= 0x80 << shift;	

	// 3.清零剩余部分
	// tail 初始化时已清零，对剩下的部分无需进行操作
	
	// 4.根据当前空余容量是否能存下 length 选择剩余操作

	auto bit_length = length << 3;	// 保存比特位的长度

	if (index < 14)
	{
		*(reinterpret_cast<uint64_t*>(&tail[14])) = bit_length;
		process(tail, hash);
	}
	else
	{
		process(tail, hash);
		block append[16]{};
		*(reinterpret_cast<uint64_t*>(&append[14])) = bit_length;
		process(append, hash);
	}

	// 5.block -> byte
	memcpy(result, hash, 16);
}

// 对 16 个 block 为一组的数据进行摘要处理
void MD5::process(const block M[16], block hash[4])
{
	block a = hash[0], b = hash[1], c = hash[2], d = hash[3];

	FF(a, b, c, d, M[0],  S11, 0xd76aa478);
	FF(d, a, b, c, M[1],  S12, 0xe8c7b756);
	FF(c, d, a, b, M[2],  S13, 0x242070db);
	FF(b, c, d, a, M[3],  S14, 0xc1bdceee);
	FF(a, b, c, d, M[4],  S11, 0xf57c0faf);
	FF(d, a, b, c, M[5],  S12, 0x4787c62a);
	FF(c, d, a, b, M[6],  S13, 0xa8304613);
	FF(b, c, d, a, M[7],  S14, 0xfd469501);
	FF(a, b, c, d, M[8],  S11, 0x698098d8);
	FF(d, a, b, c, M[9],  S12, 0x8b44f7af);
	FF(c, d, a, b, M[10], S13, 0xffff5bb1);
	FF(b, c, d, a, M[11], S14, 0x895cd7be);
	FF(a, b, c, d, M[12], S11, 0x6b901122);
	FF(d, a, b, c, M[13], S12, 0xfd987193);
	FF(c, d, a, b, M[14], S13, 0xa679438e);
	FF(b, c, d, a, M[15], S14, 0x49b40821);

	GG(a, b, c, d, M[1],  S21, 0xf61e2562);
	GG(d, a, b, c, M[6],  S22, 0xc040b340);
	GG(c, d, a, b, M[11], S23, 0x265e5a51);
	GG(b, c, d, a, M[0],  S24, 0xe9b6c7aa);
	GG(a, b, c, d, M[5],  S21, 0xd62f105d);
	GG(d, a, b, c, M[10], S22, 0x2441453);
	GG(c, d, a, b, M[15], S23, 0xd8a1e681);
	GG(b, c, d, a, M[4],  S24, 0xe7d3fbc8);
	GG(a, b, c, d, M[9],  S21, 0x21e1cde6);
	GG(d, a, b, c, M[14], S22, 0xc33707d6);
	GG(c, d, a, b, M[3],  S23, 0xf4d50d87);
	GG(b, c, d, a, M[8],  S24, 0x455a14ed);
	GG(a, b, c, d, M[13], S21, 0xa9e3e905);
	GG(d, a, b, c, M[2],  S22, 0xfcefa3f8);
	GG(c, d, a, b, M[7],  S23, 0x676f02d9);
	GG(b, c, d, a, M[12], S24, 0x8d2a4c8a);

	HH(a, b, c, d, M[5],  S31, 0xfffa3942);
	HH(d, a, b, c, M[8],  S32, 0x8771f681);
	HH(c, d, a, b, M[11], S33, 0x6d9d6122);
	HH(b, c, d, a, M[14], S34, 0xfde5380c);
	HH(a, b, c, d, M[1],  S31, 0xa4beea44);
	HH(d, a, b, c, M[4],  S32, 0x4bdecfa9);
	HH(c, d, a, b, M[7],  S33, 0xf6bb4b60);
	HH(b, c, d, a, M[10], S34, 0xbebfbc70);
	HH(a, b, c, d, M[13], S31, 0x289b7ec6);
	HH(d, a, b, c, M[0],  S32, 0xeaa127fa);
	HH(c, d, a, b, M[3],  S33, 0xd4ef3085);
	HH(b, c, d, a, M[6],  S34, 0x4881d05);
	HH(a, b, c, d, M[9],  S31, 0xd9d4d039);
	HH(d, a, b, c, M[12], S32, 0xe6db99e5);
	HH(c, d, a, b, M[15], S33, 0x1fa27cf8);
	HH(b, c, d, a, M[2],  S34, 0xc4ac5665);

	II(a, b, c, d, M[0],  S41, 0xf4292244);
	II(d, a, b, c, M[7],  S42, 0x432aff97);
	II(c, d, a, b, M[14], S43, 0xab9423a7);
	II(b, c, d, a, M[5],  S44, 0xfc93a039);
	II(a, b, c, d, M[12], S41, 0x655b59c3);
	II(d, a, b, c, M[3],  S42, 0x8f0ccc92);
	II(c, d, a, b, M[10], S43, 0xffeff47d);
	II(b, c, d, a, M[1],  S44, 0x85845dd1);
	II(a, b, c, d, M[8],  S41, 0x6fa87e4f);
	II(d, a, b, c, M[15], S42, 0xfe2ce6e0);
	II(c, d, a, b, M[6],  S43, 0xa3014314);
	II(b, c, d, a, M[13], S44, 0x4e0811a1);
	II(a, b, c, d, M[4],  S41, 0xf7537e82);
	II(d, a, b, c, M[11], S42, 0xbd3af235);
	II(c, d, a, b, M[2],  S43, 0x2ad7d2bb);
	II(b, c, d, a, M[9],  S44, 0xeb86d391);

	hash[0] += a;
	hash[1] += b;
	hash[2] += c;
	hash[3] += d;
}

// Secure Hash Algorithm 2nd ---------------------------------------------------

// SHA-256




}	// namespace toy