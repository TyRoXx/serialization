#ifndef SERIALIZATION_BIG_ENDIAN_HPP_D3149094_ADEF_4FF6_B2CF_BD0110DF06BD
#define SERIALIZATION_BIG_ENDIAN_HPP_D3149094_ADEF_4FF6_B2CF_BD0110DF06BD


#include <szn/integer.hpp>


namespace szn
{
	struct big_endian_byte_order SZN_FINAL
	{
		static std::size_t getByteShift(std::size_t byteIndex,
										std::size_t wordSize)
		{
			return wordSize - 1u - byteIndex;
		}
	};

	typedef Integer<1, big_endian_byte_order> BE8;
	typedef Integer<2, big_endian_byte_order> BE16;
	typedef Integer<4, big_endian_byte_order> BE32;
	typedef Integer<8, big_endian_byte_order> BE64;
}


#endif
