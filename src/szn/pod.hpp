#ifndef SERIALIZATION_POD_HPP
#define SERIALIZATION_POD_HPP


#include <szn/util.hpp>
#include <cassert>


namespace szn
{
	struct POD SZN_FINAL
	{
		template <class Sink, class Value>
		void serialize(Sink &sink, const Value &value) const
		{
			char const * const data = reinterpret_cast<char const *>(&value);
			assert(data);
			sink.write(data, sizeof(value));
		}

		template <class Source, class Value>
		void deserialize(Source &source, Value &value) const
		{
			char * const data = reinterpret_cast<char *>(&value);
			assert(data);
			source.read(data, sizeof(value));
		}
	};
}


#endif
