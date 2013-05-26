#ifndef SERIALIZATION_BIG_ENDIAN_HPP_D3149094_ADEF_4FF6_B2CF_BD0110DF06BD
#define SERIALIZATION_BIG_ENDIAN_HPP_D3149094_ADEF_4FF6_B2CF_BD0110DF06BD


#include "util.hpp"


namespace szn
{
	/// a format for big endian integers
	template <std::size_t SizeInBytes>
	struct BigEndian
	{
		template <class Integer>
		void serialize(Sink &sink, Integer value) const
		{
			for (std::size_t i = 0; i < SizeInBytes; ++i)
			{
				const char digit = static_cast<char>(
							(value >> ((SizeInBytes - 1u - i) * 8u)) & 0xff
							);
				sink.write(&digit, 1);
			}
		}

		template <class Integer>
		void deserialize(Source &source, Integer &value) const
		{
			Integer result = 0;
			for (std::size_t i = 0; i < SizeInBytes; ++i)
			{
				// TODO check out of range
				const char digit = source.get(i);
				result |= ((static_cast<Integer>(digit) & 0xff) <<
						   (SizeInBytes - 1u - i) * 8u);
			}
			value = result;
			source.drop(SizeInBytes);
		}
	};

	typedef BigEndian<1> BE8;
	typedef BigEndian<2> BE16;
	typedef BigEndian<4> BE32;
	typedef BigEndian<8> BE64;
}


#endif
