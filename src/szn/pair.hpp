#ifndef SERIALIZATION_VECTOR_HPP_
#define SERIALIZATION_VECTOR_HPP_


#include <utility>


namespace szn
{
	template <class First, class Second>
	struct pair
	{
		typedef std::pair<typename First::default_type, typename Second::default_type> default_type;

		template <class Sink, class FirstValue, class SecondValue>
		void serialize(Sink &sink, std::pair<FirstValue, SecondValue> const &value) const
		{
			First ().serialize(sink, value.first);
			Second().serialize(sink, value.second);
		}

		template <class Source, class FirstValue, class SecondValue>
		void deserialize(Source &source, std::pair<FirstValue, SecondValue> &value) const
		{
			First ().deserialize(source, value.first);
			Second().deserialize(source, value.second);
		}
	};
}


#endif
