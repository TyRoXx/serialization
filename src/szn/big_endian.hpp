#ifndef SERIALIZATION_BIG_ENDIAN_HPP_D3149094_ADEF_4FF6_B2CF_BD0110DF06BD
#define SERIALIZATION_BIG_ENDIAN_HPP_D3149094_ADEF_4FF6_B2CF_BD0110DF06BD


#include <szn/util.hpp>
#include <boost/integer.hpp>


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
				const unsigned char digit = static_cast<char>(
							(value >> ((SizeInBytes - 1u - i) * 8u)) & 0xff
							);
				sink.write(reinterpret_cast<char const *>(&digit), 1);
			}
		}

		template <class Integer>
		void deserialize(Source &source, Integer &value) const
		{
			typedef typename boost::uint_t<sizeof(value) * CHAR_BIT>::fast
				AlgorithmicInteger;

			AlgorithmicInteger result = 0;
			for (std::size_t i = 0; i < SizeInBytes; ++i)
			{
				// TODO check out of range
				const unsigned char digit = source.get(i);
				result |= static_cast<AlgorithmicInteger>(
						   (
							(static_cast<AlgorithmicInteger>(digit) & 0xff) <<
							 ((SizeInBytes - 1u - i) * 8)
						   )
						  );
			}
			value = static_cast<Integer>(result);
			source.drop(SizeInBytes);
		}
	};

	typedef BigEndian<1> BE8;
	typedef BigEndian<2> BE16;
	typedef BigEndian<4> BE32;
	typedef BigEndian<8> BE64;
}


#endif
