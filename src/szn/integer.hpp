#ifndef SERIALIZATION_INTEGER_HPP_C1F7C3A8_232A_4C8B_83AD_B1BA72632C85
#define SERIALIZATION_INTEGER_HPP_C1F7C3A8_232A_4C8B_83AD_B1BA72632C85


#include <szn/util.hpp>
#include <boost/integer.hpp>
#include <boost/type_traits/make_unsigned.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <boost/utility/declval.hpp>


namespace szn
{
	namespace detail
	{
		template <std::size_t Size>
		struct intrinsic_size_tag
		{
		};

		template <class Sink, class Integer, class ByteOrder, std::size_t IntegerSize>
		struct integer_serializer SZN_FINAL
		{
			Sink *sink;
			Integer value;

			template <class SizeTag>
			typename boost::enable_if_c<!boost::is_same<void, decltype(ByteOrder::make_serializable_pod(boost::declval<Integer>(), SizeTag()))>::value, void>::type
			serialize(SizeTag, int) const
			{
				assert(sink);
				const Integer prepared_for_writing = ByteOrder::make_serializable_pod(value, intrinsic_size_tag<IntegerSize>());
				BOOST_STATIC_ASSERT(sizeof(prepared_for_writing) == IntegerSize);
				sink->write(reinterpret_cast<char const *>(&prepared_for_writing), IntegerSize);
			}

			template <class SizeTag>
			void serialize(SizeTag, ...) const
			{
				assert(sink);
				//Value could be an enum class which is not implicitly convertible
				//to any integer. We have to do the conversion explicitly.
				typedef typename boost::uint_t<sizeof(value) * 8>::least ArithUValue;
				ArithUValue const arith_value = static_cast<ArithUValue>(value);

				for (std::size_t b = 0; b < IntegerSize; ++b)
				{
					std::size_t const byte_shift = ByteOrder::get_byte_shift(b, IntegerSize);
					char const byte = static_cast<unsigned char>(arith_value >> (byte_shift * 8));
					sink->write(&byte, 1);
				}
			}
		};
	}

	template <std::size_t ByteSize, class ByteOrder>
	struct integer SZN_FINAL
	{
		static length_type const min_size = ByteSize;
		static length_type const max_size = ByteSize;

		typedef typename boost::uint_t<ByteSize * 8>::least default_type;

		template <class Sink, class Value>
		void serialize(Sink &sink, Value value) const
		{
			detail::integer_serializer<Sink, default_type, ByteOrder, ByteSize> serializer;
			serializer.sink = &sink;
			serializer.value = static_cast<default_type>(value);
			serializer.serialize(detail::intrinsic_size_tag<ByteSize>(), 0);
		}

		template <class Source, class Value>
		void deserialize(Source &source, Value &value) const
		{
			typedef default_type UValue;
			UValue result = 0;
			for (std::size_t b = 0; b < ByteSize; ++b)
			{
				unsigned char byte;
				source.read(reinterpret_cast<char *>(&byte), 1);
				std::size_t const byte_shift = ByteOrder::get_byte_shift(b, ByteSize);
				result = static_cast<UValue>(result | static_cast<UValue>(static_cast<UValue>(byte) << static_cast<UValue>(byte_shift * 8)));
			}

			//If Value is signed and ByteSize is less than sizeof(Value)
			//we have to be careful with the final cast.
			//In order to get correct negative values out we have to sign-extend
			//the intermediate result. This can be achieved with a cast to
			//a signed integer type of the same size, followed by a widening
			//and sign-entending cast to the Value type.
			//Enums can be signed, but it makes no sense to sign-extend them.
			if (boost::is_signed<Value>::value &&
				!boost::is_enum<Value>::value)
			{
				typedef typename boost::int_t<ByteSize * 8>::least SignExtender;
				SignExtender const signed_result = static_cast<SignExtender>(result);
				value = static_cast<Value>(signed_result);
			}
			else
			{
				value = static_cast<Value>(result);
			}
		}
	};
}


#endif
