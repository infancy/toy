#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TOY_SECURE_RSA_H
#define TOY_SECURE_RSA_H

#include <iostream>
#include <cmath>

namespace toy
{

long RSA(long baseNum, long message, long key) 
{
	if (baseNum < 1 || key < 1)
		return 0L;

	return static_cast<long>(pow(message, key)) % baseNum;
}

void RSA_test() 
{
	//基数  
	int baseNum = 3 * 11;
	//公钥  
	int keyE = 3;
	//密钥  
	int keyD = 7;
	//未加密的数据  
	long msg = 24L;
	//加密后的数据  
	long encode_msg = RSA(baseNum, keyE, msg);
	//解密后的数据  
	long decode_msg = RSA(baseNum, keyD, encode_msg);

	std::cout << "加密前：" << msg;
	std::cout << "加密后：" << encode_msg;
	std::cout << "解密后：" << decode_msg;
}

}	// namespace toy	

#endif	// TOY_SECURE_RSA_H