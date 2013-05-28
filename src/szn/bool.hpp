#ifndef SERIALIZATION_BOOL_HPP
#define SERIALIZATION_BOOL_HPP


#include <szn/big_endian.hpp>


namespace szn
{
	template <class Format, class ValuePolicy>
	struct BasicBool
	{
		static std::size_t const minSize = MinSize<Format>::value;
		static std::size_t const maxSize = MaxSize<Format>::value;

		typedef bool default_type;

		template <class Sink>
		void serialize(Sink &sink, bool value) const
		{
			Format().serialize(sink, ValuePolicy::fromBool(value));
		}

		template <class Source>
		void deserialize(Source &source, bool &value) const
		{
			typename ValuePolicy::value_type buffer;
			Format().deserialize(source, buffer);
			value = ValuePolicy::toBool(buffer);
		}
	};

	struct ByteBool
	{
		typedef unsigned char value_type;

		static value_type fromBool(bool value)
		{
			return (value ? 1 : 0);
		}

		static bool toBool(value_type value)
		{
			return (value != 0);
		}
	};

	typedef BasicBool<BE8, ByteBool> Bool;
}


#endif
