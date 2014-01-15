#include "sha-256.hpp"
#include <algorithm>

static inline std::uint64_t ByteSwap(std::uint64_t x)
{
	return
		(0xff00000000000000ull & (x << 0x38)) |
		(0x00ff000000000000ull & (x << 0x28)) |
		(0x0000ff0000000000ull & (x << 0x18)) |
		(0x000000ff00000000ull & (x << 0x08)) |
		(0x00000000ff000000ull & (x >> 0x08)) |
		(0x0000000000ff0000ull & (x >> 0x18)) |
		(0x000000000000ff00ull & (x >> 0x28)) |
		(0x00000000000000ffull & (x >> 0x38));
}

static inline std::uint32_t ByteSwap(std::uint32_t x)
{
	return
		(0xff000000u & (x << 0x18)) |
		(0x00ff0000u & (x << 0x08)) |
		(0x0000ff00u & (x >> 0x08)) |
		(0x000000ffu & (x >> 0x18));
}

template<typename T>
struct CByteSwap
{
	T operator()(T x) { return ByteSwap(x); }
};

static const std::uint32_t H0[8] = {
	0x6a09e667u,
	0xbb67ae85u,
	0x3c6ef372u,
	0xa54ff53au,
	0x510e527fu,
	0x9b05688cu,
	0x1f83d9abu,
	0x5be0cd19u
};

static const std::uint32_t K256[64] = {
	0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu, 0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u,
	0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u,
	0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu, 0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
	0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u, 0x06ca6351u, 0x14292967u,
	0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
	0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
	0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu, 0x682e6ff3u,
	0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u, 0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u
};

static inline std::uint32_t Ch(std::uint32_t x, std::uint32_t y, std::uint32_t z)
{
	return (x & y) ^ (~x & z);
}

static inline std::uint32_t Maj(std::uint32_t x, std::uint32_t y, std::uint32_t z)
{
	return (x & y) ^ (x & z) ^ (y & z);
}

static inline std::uint32_t ROTR(int n, std::uint32_t x)
{
	return (x >> n) | (x << 32 - n);
}

static inline std::uint32_t SHR(int n, std::uint32_t x)
{
	return x >> n;
}

static inline std::uint32_t Sum256_0(std::uint32_t x)
{
	return ROTR(2, x) ^ ROTR(13, x) ^ ROTR(22, x);
}

static inline std::uint32_t Sum256_1(std::uint32_t x)
{
	return ROTR(6, x) ^ ROTR(11, x) ^ ROTR(25, x);
}

static inline std::uint32_t Sigma256_0(std::uint32_t x)
{
	return ROTR(7, x) ^ ROTR(18, x) ^ SHR(3, x);
}

static inline std::uint32_t Sigma256_1(std::uint32_t x)
{
	return ROTR(17, x) ^ ROTR(19, x) ^ SHR(10, x);
}

CSha256::CSha256(CSha256::U* hash)
	: hash_(reinterpret_cast<std::uint32_t*>(hash))
	, messageSize_(0)
{
	std::copy(H0, H0 + 8, hash_);
}

void CSha256::TransformBlock(const CSha256::U block[CSha256::B])
{
	messageSize_ += B;
	const std::uint32_t* M = reinterpret_cast<const std::uint32_t*>(block);

	std::array<std::uint32_t, 64> W;
	std::transform(M, M + 16, W.begin(), CByteSwap<std::uint32_t>());
	for (int t = 16; t < 64; ++t)
	{
		W[t] = Sigma256_1(W[t - 2]) + W[t - 7] + Sigma256_0(W[t - 15]) + W[t - 16];
	}

	std::array<std::uint32_t, 8> A;
	std::copy_n(hash_, 8, A.begin());

	for (int t = 0; t < 64; ++t)
	{
		uint32_t T1 = A[7] + Sum256_1(A[4]) + Ch(A[4], A[5], A[6]) + K256[t] + W[t];
		uint32_t T2 = Sum256_0(A[0]) + Maj(A[0], A[1], A[2]);
		A[7] = A[6];
		A[6] = A[5];
		A[5] = A[4];
		A[4] = A[3] + T1;
		A[3] = A[2];
		A[2] = A[1];
		A[1] = A[0];
		A[0] = T1 + T2;
	}

	std::transform(hash_, hash_ + 8, A.begin(), hash_, std::plus<std::uint32_t>());
}

CSha256::U* CSha256::TransfromFinal(const CSha256::U block[CSha256::B], std::uint64_t size)
{
	std::array<U, B * 2> padding = { 0 };
	std::copy_n(block, size, padding.begin());
	padding[size] = 0x80;

	if (size <= 55ULL) {
		*reinterpret_cast<std::uint64_t*>(&padding[B - 8]) = ByteSwap((messageSize_ + size) * 8);
		TransformBlock(padding.data());
	} else {
		*reinterpret_cast<std::uint64_t*>(&padding[2 * B - 8]) = ByteSwap((messageSize_ + size) * 8);
		TransformBlock(padding.data());
		TransformBlock(padding.data() + B);
	}
	std::transform(hash_, hash_ + 8, hash_, CByteSwap<std::uint32_t>());
	return reinterpret_cast<CSha256::U*>(hash_);
}
