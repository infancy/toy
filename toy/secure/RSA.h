#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TOY_SECURE_RSA_H
#define TOY_SECURE_RSA_H

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iostream>
#include <utility>
#include <vector>

namespace toy
{

// https://zh.wikipedia.org/wiki/RSA%E5%8A%A0%E5%AF%86%E6%BC%94%E7%AE%97%E6%B3%95
// https://github.com/pantaloons/RSA/blob/master/single.c

class RSA
{
public:
	void generate_key();

	std::pair<size_t, size_t> public_key()  { return pub_key; }	// (N, e)
	std::pair<size_t, size_t> private_key() { return pri_key; }	// (N, d)

private:
	// size_t bytes{};		// 每次处理的字节数

	std::pair<size_t, size_t> pub_key{};
	std::pair<size_t, size_t> pri_key{};
};

std::vector<int> RSA_encode(std::string plaintext, std::pair<size_t, size_t> public_key);

std::string RSA_decode(const std::vector<int>& ciphertext, std::pair<size_t, size_t> private_key);

}	// namespace toy	

#endif	// TOY_SECURE_RSA_H