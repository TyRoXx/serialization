#ifndef SERIALIZATION_LITTLE_ENDIAN_HPP_BDA05921_80B7_4DF0_BBC7_B73C484EC091
#define SERIALIZATION_LITTLE_ENDIAN_HPP_BDA05921_80B7_4DF0_BBC7_B73C484EC091


#include <szn/integer.hpp>


namespace szn
{
	struct little_endian_byte_order SZN_FINAL
	{
		static std::size_t getByteShift(std::size_t byteIndex,
										std::size_t wordSize)
		{
			(void)wordSize;
			return byteIndex;
		}
	};

	typedef integer<1, little_endian_byte_order> LE8;
	typedef integer<2, little_endian_byte_order> LE16;
	typedef integer<4, little_endian_byte_order> LE32;
	typedef integer<8, little_endian_byte_order> LE64;
}


#endif
