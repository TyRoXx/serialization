#ifndef SERIALIZATION_LITTLE_ENDIAN_H
#define SERIALIZATION_LITTLE_ENDIAN_H


#include "util.h"


namespace szn
{
	/// a format for little endian integers
	template <std::size_t SizeInBytes>
	struct LittleEndian
	{
		template <class Integer>
		void serialize(Sink &sink, Integer value) const
		{
//			BOOST_STATIC_ASSERT(sizeof(value) <= SizeInBytes);

			for (std::size_t i = 0; i < SizeInBytes; ++i)
			{
				const char digit = static_cast<char>(value);
				sink.write(&digit, 1);
				value >>= 8;
			}
		}

		template <class Integer>
		void deserialize(Source &source, Integer &value) const
		{
			Integer result = 0;
			for (std::size_t i = 0; i < SizeInBytes; ++i)
			{
				// TODO check out of range
				// TODO prevent integer overflow
				const char digit = source.get(i);
				result |= ((static_cast<Integer>(digit) & 0xff) << (i * 8));
			}
			value = result;
			source.drop(SizeInBytes);
		}
	};

	typedef LittleEndian<1> LE8;
	typedef LittleEndian<2> LE16;
	typedef LittleEndian<4> LE32;
	typedef LittleEndian<8> LE64;
}


#endif
