#ifndef SERIALIZATION_POD_HPP_5BFB20F4_B539_4183_9314_8101D34C5CED
#define SERIALIZATION_POD_HPP_5BFB20F4_B539_4183_9314_8101D34C5CED


#include <szn/util.hpp>


namespace szn
{
	template <class Value>
	struct pod SZN_FINAL
	{
		static length_type const min_size = sizeof(Value);
		static length_type const max_size = sizeof(Value);

		typedef Value default_type;

		template <class Sink>
		void serialize(Sink &sink, const Value &value) const
		{
			char const * const data = reinterpret_cast<char const *>(&value);
			sink.write(data, sizeof(value));
		}

		template <class Source>
		void deserialize(Source &source, Value &value) const
		{
			char * const data = reinterpret_cast<char *>(&value);
			source.read(data, sizeof(value));
		}
	};
}


#endif
