#include "toy/secure/RSA.h"
#include <iostream>

using namespace std;

namespace toy
{

namespace
{	// support functions

const int ACCURACY = 5;
const int SINGLE_MAX = 10000;
const int EXPONENT_MAX = 1000;
const int BUF_SIZE = 1024;

/**
* Computes a^b mod c
*/
int modpow(long long a, long long b, int c) {
	int res = 1;
	while (b > 0) {
		/* Need long multiplication else this will overflow... */
		if (b & 1) {
			res = (res * a) % c;
		}
		b = b >> 1;
		a = (a * a) % c; /* Same deal here */
	}
	return res;
}

/**
* Computes the Jacobi symbol, (a, n)
*/
int jacobi(int a, int n) {
	int twos, temp;
	int mult = 1;
	while (a > 1 && a != n) {
		a = a % n;
		if (a <= 1 || a == n) break;
		twos = 0;
		while (a % 2 == 0 && ++twos) a /= 2; /* Factor out multiples of 2 */
		if (twos > 0 && twos % 2 == 1) mult *= (n % 8 == 1 || n % 8 == 7) * 2 - 1;
		if (a <= 1 || a == n) break;
		if (n % 4 != 1 && a % 4 != 1) mult *= -1; /* Coefficient for flipping */
		temp = a;
		a = n;
		n = temp;
	}
	if (a == 0) return 0;
	else if (a == 1) return mult;
	else return 0; /* a == n => gcd(a, n) != 1 */
}

/**
* Check whether a is a Euler witness for n
*/
int solovayPrime(int a, int n) {
	int x = jacobi(a, n);
	if (x == -1) x = n - 1;
	return x != 0 && modpow(a, (n - 1) / 2, n) == x;
}

/**
* Test if n is probably prime, using accuracy of k (k solovay tests)
*/
int probablePrime(int n, int k) {
	if (n == 2) return 1;
	else if (n % 2 == 0 || n == 1) return 0;
	while (k-- > 0) {
		if (!solovayPrime(rand() % (n - 2) + 2, n)) return 0;
	}
	return 1;
}

/**
* Find a random (probable) prime between 3 and n - 1, this distribution is
* nowhere near uniform, see prime gaps
*/
int rand_prime(int n)
{
	int prime = rand() % n;
	n += n % 2; /* n needs to be even so modulo wrapping preserves oddness */
	prime += 1 - prime % 2;
	while (1) {
		if (probablePrime(prime, ACCURACY)) return prime;
		prime = (prime + 2) % n;
	}
}

int gcd(int a, int b) {
	int temp;
	while (b != 0) {
		temp = b;
		b = a % b;
		a = temp;
	}
	return a;
}

/**
* Find a random exponent x between 3 and n - 1 such that gcd(x, phi) = 1,
* this distribution is similarly nowhere near uniform
*/
int rand_exponent(int phi, int n) {
	int e = rand() % n;
	while (1) {
		if (gcd(e, phi) == 1) return e;
		e = (e + 1) % n;
		if (e <= 2) e = 3;
	}
}

/**
* Compute n^-1 mod m by extended euclidian method
*/
int inverse(int n, int modulus) {
	int a = n, b = modulus;
	int x = 0, y = 1, x0 = 1, y0 = 0, q, temp;
	while (b != 0) {
		q = a / b;
		temp = a % b;
		a = b;
		b = temp;
		temp = x; x = x0 - q * x; x0 = temp;
		temp = y; y = y0 - q * y; y0 = temp;
	}
	if (x0 < 0) x0 += modulus;
	return x0;
}

/**
* Encode the message m using public exponent and modulus, c = m^e mod n
*/
int encode(int m, int e, int n) {
	return modpow(m, e, n);
}

/**
* Decode cryptogram c using private exponent and public modulus, m = c^d mod n
*/
int decode(int c, int d, int n) {
	return modpow(c, d, n);
}

}	// namespace

void RSA::generate_key()
{
	srand(time(nullptr));

	size_t p{}, q{}, n{};

	while (true)
	{
		// 1.two prime factors
		p = rand_prime(SINGLE_MAX);
		q = rand_prime(SINGLE_MAX);

		// 2.prime product
		n = p * q;
		// at least prime product should more than 128
		// to encode single bytes.
		if (n >= 128)
			break;
	}

	// 3.totient
	size_t phi = (p - 1) * (q - 1);

	// 4.Chose public exponent
	// e > 1 && e < totient && gcd(e, totient) = 1
	size_t e = rand_exponent(phi, EXPONENT_MAX);
	pub_key = { n, e };

	// 5.Calculated private exponent
	// for certain n and e, only one d can uesd as pri_key
	size_t d = inverse(e, phi);
	pri_key = { n, d };
}

vector<int> RSA_encode(string plaintext, pair<size_t, size_t> public_key)
{
	auto n = public_key.first, e = public_key.second;

	size_t bytes{};
	if (n >> 21) bytes = 3;
	else if (n >> 14) bytes = 2;
	else bytes = 1;

	// 避免了零长度？
	auto pad_len = bytes - plaintext.length() % bytes;
	for(size_t i = 0; i < pad_len; ++i)
		// plaintext.append('\0');
		plaintext.push_back('\0');

	size_t cipher_len = plaintext.length() / bytes;
	vector<int> ciphertext(cipher_len);

	for (size_t i = 0; i < cipher_len; ++i)
	{
		// given by m = (m1 + m2 * 128 + m3 * 128 ^ 2 + ..),
		// cipher[i] = m^e mod n
		int m = 0;
		for (int j = 0; j < bytes; ++j)
		{
			// cout << '\"' << plaintext[i + j] << "\"\n";
			m += plaintext[i*bytes + j] * (1 << (7 * j));
		}
		ciphertext[i] = encode(m, e, n);
	}

	return ciphertext;
}

string RSA_decode(const vector<int>& ciphertext, pair<size_t, size_t> private_key)
{
	auto n = private_key.first, d = private_key.second;

	size_t bytes{};
	if (n >> 21) bytes = 3;
	else if (n >> 14) bytes = 2;
	else bytes = 1;

	auto cipher_len = ciphertext.size();
	string plaintext(cipher_len * bytes, '\0');

	for (int i = 0; i < cipher_len; ++i)
	{
		int m = decode(ciphertext[i], d, n);

		for (int j = 0; j < bytes; ++j)
			plaintext[i*bytes + j] = static_cast<char>((m >> (7 * j)) % 128);
	}

	// 这样判断 pad 的实现存在问题
	while (!plaintext.empty() && plaintext.back() == '\0')
		plaintext.pop_back();

	return plaintext;
}

}	// namespace toy