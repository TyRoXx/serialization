#ifndef SERIALIZATION_BOOL_HPP_57D2463B_8AA8_479C_94E3_BB6555E9C59B
#define SERIALIZATION_BOOL_HPP_57D2463B_8AA8_479C_94E3_BB6555E9C59B


#include <szn/big_endian.hpp>
#include <szn/format.hpp>


namespace szn
{
	template <class Format, class ValuePolicy>
	struct basic_bool
	{
		static length_type const min_size = min_size<Format>::value;
		static length_type const max_size = max_size<Format>::value;

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

	struct byte_bool
	{
		typedef unsigned char value_type;

		static value_type fromBool(bool value)
		{
			//explicit cast to silence warning by GCC 4.3
			return static_cast<value_type>(value ? 1 : 0);
		}

		static bool toBool(value_type value)
		{
			return (value != 0);
		}
	};

	typedef basic_bool<be8, byte_bool> boolean;
}


#endif
