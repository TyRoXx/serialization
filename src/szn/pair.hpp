#ifndef SERIALIZATION_VECTOR_HPP_
#define SERIALIZATION_VECTOR_HPP_


#include <utility>


namespace szn
{
	namespace pair_order
	{
		enum type
		{
			first_second,
			second_first
		};
	}

	template <class First, class Second, pair_order::type Order = pair_order::first_second>
	struct pair
	{
		typedef std::pair<typename First::default_type, typename Second::default_type> default_type;

		template <class Sink, class FirstValue, class SecondValue>
		void serialize(Sink &sink, std::pair<FirstValue, SecondValue> const &value) const
		{
			if (Order == pair_order::first_second)
			{
				First().serialize(sink, value.first);
			}
			Second().serialize(sink, value.second);
			if (Order != pair_order::first_second)
			{
				First().serialize(sink, value.first);
			}
		}

		template <class Source, class FirstValue, class SecondValue>
		void deserialize(Source &source, std::pair<FirstValue, SecondValue> &value) const
		{
			if (Order == pair_order::first_second)
			{
				First().deserialize(source, value.first);
			}
			Second().deserialize(source, value.second);
			if (Order != pair_order::first_second)
			{
				First().deserialize(source, value.first);
			}
		}
	};
}


#endif
