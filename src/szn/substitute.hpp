#ifndef SERIALIZATION_SUBSTITUTE_HPP_
#define SERIALIZATION_SUBSTITUTE_HPP_


#include <szn/format.hpp>
#include <stdexcept>


namespace szn
{
	template <class ValueType, ValueType Value>
	struct substitute SZN_FINAL
	{
		static length_type const min_size = 0;
		static length_type const max_size = 0;

		typedef ValueType default_type;

		template <class Sink>
		void serialize(Sink &sink, ValueType const &value) const
		{
			(void)sink;
			if (value != Value)
			{
				throw std::invalid_argument("Substitute value mismatch");
			}
		}

		template <class Source>
		void deserialize(Source &source, ValueType &value) const
		{
			(void)source;
			value = Value;
		}
	};
}


#endif
