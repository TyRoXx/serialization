#ifndef SERIALIZATION_INTEGER_HPP
#define SERIALIZATION_INTEGER_HPP


#include <szn/util.hpp>
#include <boost/integer.hpp>


namespace szn
{
	template <std::size_t ByteSize, class ByteOrder>
	struct Integer SZN_FINAL
	{
		static std::size_t const minSize = ByteSize;
		static std::size_t const maxSize = ByteSize;

		template <class Sink, class Value>
		void serialize(Sink &sink, Value value) const
		{
			//Value could be an enum class which is not implicitly convertible
			//to any integer. We have to do the conversion explicitly.
			typedef typename boost::uint_t<sizeof(value) * 8>::least ArithUValue;
			ArithUValue const arithValue = static_cast<ArithUValue>(value);

			for (std::size_t b = 0; b < ByteSize; ++b)
			{
				std::size_t const byteShift = ByteOrder::getByteShift(b, ByteSize);
				char const byte = static_cast<unsigned char>(arithValue >> (byteShift * 8));
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
			if (std::is_signed<Value>::value)
			{
				typedef typename boost::int_t<ByteSize * 8>::least SignExtender;
				SignExtender const signedResult = static_cast<SignExtender>(result);
				value = static_cast<Value>(signedResult);
			}
			else
			{
				value = static_cast<Value>(result);
			}
		}
	};
}


#endif
