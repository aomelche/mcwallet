#ifndef __ed958bf4_495a_4995_bfef_c7e212122595
#define __ed958bf4_495a_4995_bfef_c7e212122595

#include <array>
#include <vector>
#include <cstdint>

class CSha256
{
public:
	typedef std::uint8_t U;

	static const std::uint64_t BW = 16; // Size of message block in DWORDs
	static const std::uint64_t LW = 8; // Size of hash in DWORDs
	static const std::uint64_t B = BW * 4; // Size of message block in bytes
	static const std::uint64_t L = LW * 4; // Size of hash in bytes

	template<typename T>
	static std::vector<U> Transform(const T& message)
	{
		return Transform(message.begin(), message.end());
	}

	template<typename InputIterator>
	static std::vector<U> Transform(InputIterator messageBegin, InputIterator messageEnd)
	{
		std::vector<U> result(L);
		CSha256 hash(result.data());
		std::array<U, B> block;
		std::uint64_t i = 0;
		while (messageBegin != messageEnd)
		{
			if (B == i) {
				i = 0;
				hash.TransformBlock(block.data());
			}
			block[i] = *messageBegin;
			++messageBegin;
			++i;
		}
		hash.TransfromFinal(block.data(), i);
		return result;
	}

	explicit CSha256(U* hash);
	void TransformBlock(const U block[B]);
	U* TransfromFinal(const U block[B], std::uint64_t size);

private:
	std::uint32_t* hash_;
	std::uint64_t messageSize_;
};

#endif //__ed958bf4_495a_4995_bfef_c7e212122595
