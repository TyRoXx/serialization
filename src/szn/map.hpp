#ifndef SERIALIZATION_MAP_HPP_
#define SERIALIZATION_MAP_HPP_


#include <szn/vector.hpp>
#include <szn/pair.hpp>


namespace szn
{
	template <class LengthFormat, class KeyFormat, class ValueFormat>
	struct map : vector<LengthFormat, pair<KeyFormat, ValueFormat>>
	{
	};
}


#endif
