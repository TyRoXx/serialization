#ifndef SERIALIZATION_LITTLE_ENDIAN_HPP_BDA05921_80B7_4DF0_BBC7_B73C484EC091
#define SERIALIZATION_LITTLE_ENDIAN_HPP_BDA05921_80B7_4DF0_BBC7_B73C484EC091


#include <szn/integer.hpp>


namespace szn
{
	struct little_endian_byte_order SZN_FINAL
	{
		static std::size_t get_byte_shift(std::size_t byte_index,
		                                  std::size_t word_size)
		{
			(void)word_size;
			return byte_index;
		}
	};

	typedef integer<1, little_endian_byte_order> le8;
	typedef integer<2, little_endian_byte_order> le16;
	typedef integer<4, little_endian_byte_order> le32;
	typedef integer<8, little_endian_byte_order> le64;
}


#endif
