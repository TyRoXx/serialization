#ifndef SERIALIZATION_OPTIONAL_HPP_
#define SERIALIZATION_OPTIONAL_HPP_


#include <szn/bool.hpp>
#include <szn/format.hpp>
#include <boost/optional.hpp>


namespace szn
{
	template <class ConditionFormat, class ValueFormat>
	struct optional
	{
		typedef boost::optional<typename ValueFormat::default_type> default_type;

		static length_type const min_size = add_lengths<min_size<ConditionFormat>::value, min_size<ValueFormat>::value>::value;
		static length_type const max_size = add_lengths<max_size<ConditionFormat>::value, max_size<ValueFormat>::value>::value;

		template <class Sink, class Nullable>
		void serialize(Sink &sink, Nullable const &value) const
		{
			bool const is_set = !!value;
			ConditionFormat().serialize(sink, is_set);
			if (is_set)
			{
				ValueFormat().serialize(sink, *value);
			}
		}

		template <class Source, class Value>
		void deserialize(Source &source, boost::optional<Value> &value) const
		{
			assert(!value);
			bool is_set = false;
			ConditionFormat().deserialize(source, is_set);
			if (is_set)
			{
				Value temporary;
				ValueFormat().deserialize(source, temporary);
				value = boost::move(temporary);
			}
		}
	};
}


#endif
