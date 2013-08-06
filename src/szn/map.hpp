#ifndef SERIALIZATION_MAP_HPP_
#define SERIALIZATION_MAP_HPP_


#include <szn/vector.hpp>
#include <szn/pair.hpp>


namespace szn
{
	template <class LengthFormat, class KeyFormat, class ValueFormat, class KeyValueFormat = pair<KeyFormat, ValueFormat>>
	struct map : vector<LengthFormat, KeyValueFormat>
	{
	};
}


#endif
