#ifndef SERIALIZATION_ENDIANNESS_HPP
#define SERIALIZATION_ENDIANNESS_HPP


#include <szn/util.hpp>
#include <boost/integer.hpp>


namespace szn
{
	template <std::size_t ByteSize, class ByteOrder>
	struct BasicInteger
	{
		template <class Sink, class Value>
		void serialize(Sink &sink, Value value) const
		{
			for (std::size_t b = 0; b < ByteSize; ++b)
			{
				std::size_t const byteShift = ByteOrder::getByteShift(b, ByteSize);
				char const byte = static_cast<unsigned char>(value >> (byteShift * 8));
				sink.write(&byte, 1);
			}
		}

		template <class Source, class Value>
		void deserialize(Source &source, Value &value) const
		{
			//We do the calculations with unsigned integers to keep the code simple.
			typedef typename std::make_unsigned<Value>::type UValue;

			UValue result = 0;
			for (std::size_t b = 0; b < ByteSize; ++b)
			{
				unsigned char byte;
				source.read(reinterpret_cast<char *>(&byte), 1);
				std::size_t const byteShift = ByteOrder::getByteShift(b, ByteSize);
				result |= static_cast<UValue>(static_cast<UValue>(byte) << (byteShift * 8));
			}

			//If Value is signed and ByteSize is less than sizeof(Value)
			//we have to be careful with the final cast.
			//In order to get correct negative values out we have to sign-extend
			//the intermediate result. This can be achieved with a cast to
			//a signed integer type of the same size, followed by a widening
			//and sign-entending cast to the Value type.
			typedef typename boost::int_t<ByteSize * 8>::least SignExtender;
			SignExtender const signedResult = static_cast<SignExtender>(result);
			value = static_cast<Value>(signedResult);
		}
	};

	struct LittleEndianByteOrder
	{
		static std::size_t getByteShift(std::size_t byteIndex,
										std::size_t wordSize)
		{
			(void)wordSize;
			return byteIndex;
		}
	};

	struct BigEndianByteOrder
	{
		static std::size_t getByteShift(std::size_t byteIndex,
										std::size_t wordSize)
		{
			return wordSize - 1u - byteIndex;
		}
	};

	typedef BasicInteger<1, LittleEndianByteOrder> LE_8;
	typedef BasicInteger<2, LittleEndianByteOrder> LE_16;
	typedef BasicInteger<4, LittleEndianByteOrder> LE_32;
	typedef BasicInteger<8, LittleEndianByteOrder> LE_64;

	typedef BasicInteger<1, BigEndianByteOrder> BE_8;
	typedef BasicInteger<2, BigEndianByteOrder> BE_16;
	typedef BasicInteger<4, BigEndianByteOrder> BE_32;
	typedef BasicInteger<8, BigEndianByteOrder> BE_64;
}


#endif
