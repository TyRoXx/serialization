#ifndef SERIALIZATION_POD_HPP_5BFB20F4_B539_4183_9314_8101D34C5CED
#define SERIALIZATION_POD_HPP_5BFB20F4_B539_4183_9314_8101D34C5CED


#include <szn/util.hpp>
#include <cassert>


namespace szn
{
	template <class Value>
	struct pod SZN_FINAL
	{
		static std::size_t const min_size = sizeof(Value);
		static std::size_t const max_size = sizeof(Value);

		typedef Value default_type;

		template <class Sink>
		void serialize(Sink &sink, const Value &value) const
		{
			char const * const data = reinterpret_cast<char const *>(&value);
			assert(data);
			sink.write(data, sizeof(value));
		}

		template <class Source>
		void deserialize(Source &source, Value &value) const
		{
			char * const data = reinterpret_cast<char *>(&value);
			assert(data);
			source.read(data, sizeof(value));
		}
	};
}


#endif
