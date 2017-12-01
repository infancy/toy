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
	//����  
	int baseNum = 3 * 11;
	//��Կ  
	int keyE = 3;
	//��Կ  
	int keyD = 7;
	//δ���ܵ�����  
	long msg = 24L;
	//���ܺ������  
	long encode_msg = RSA(baseNum, keyE, msg);
	//���ܺ������  
	long decode_msg = RSA(baseNum, keyD, encode_msg);

	std::cout << "����ǰ��" << msg;
	std::cout << "���ܺ�" << encode_msg;
	std::cout << "���ܺ�" << decode_msg;
}

}	// namespace toy	

#endif	// TOY_SECURE_RSA_H