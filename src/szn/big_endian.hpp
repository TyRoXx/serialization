#ifndef SERIALIZATION_BIG_ENDIAN_HPP_D3149094_ADEF_4FF6_B2CF_BD0110DF06BD
#define SERIALIZATION_BIG_ENDIAN_HPP_D3149094_ADEF_4FF6_B2CF_BD0110DF06BD


#include <szn/integer.hpp>
#include <szn/intrinsics.hpp>


namespace szn
{
	struct big_endian_byte_order SZN_FINAL
	{
		static std::size_t get_byte_shift(std::size_t byte_index,
		                                  std::size_t word_size)
		{
			return word_size - 1u - byte_index;
		}

		template <class T>
		static T make_serializable_pod(T value, detail::intrinsic_size_tag<2>)
		{
			return intrinsics::hton16(value);
		}

		template <class T>
		static T make_serializable_pod(T value, detail::intrinsic_size_tag<4>)
		{
			return intrinsics::hton32(value);
		}

		//TODO: 64 bit
	};

	typedef integer<1, big_endian_byte_order> be8;
	typedef integer<2, big_endian_byte_order> be16;
	typedef integer<4, big_endian_byte_order> be32;
	typedef integer<8, big_endian_byte_order> be64;
}


#endif
