#ifndef SERIALIZATION_LITTLE_ENDIAN_HPP_BDA05921_80B7_4DF0_BBC7_B73C484EC091
#define SERIALIZATION_LITTLE_ENDIAN_HPP_BDA05921_80B7_4DF0_BBC7_B73C484EC091


#include <szn/integer.hpp>


namespace szn
{
	struct LittleEndianByteOrder SZN_FINAL
	{
		static std::size_t getByteShift(std::size_t byteIndex,
										std::size_t wordSize)
		{
			(void)wordSize;
			return byteIndex;
		}
	};

	typedef Integer<1, LittleEndianByteOrder> LE8;
	typedef Integer<2, LittleEndianByteOrder> LE16;
	typedef Integer<4, LittleEndianByteOrder> LE32;
	typedef Integer<8, LittleEndianByteOrder> LE64;
}


#endif
