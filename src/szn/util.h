#ifndef SERIALIZATION_UTIL_H
#define SERIALIZATION_UTIL_H


#include <cstddef>
#include <utility>


#define SZN_FINAL final
#define SZN_OVERRIDE override


namespace szn
{
	struct Sink;
	struct Source;


	/// just an alternative way of calling format.serialize
	template <class Value, class Format>
	void serialize(Sink &sink, const Value &value, const Format &format)
	{
		format.serialize(sink, value);
	}

	/// just an alternative way of calling format.deserialize
	template <class Value, class Format>
	void deserialize(Source &source, Value &value, const Format &format)
	{
		format.deserialize(source, value);
	}

	/// a format for structures with serialize/deserialize methods
	struct ByMethod SZN_FINAL
	{
		template <class Value>
		void serialize(Sink &sink, const Value &value) const
		{
			value.serialize(sink);
		}

		template <class Value>
		void deserialize(Source &source, Value &value) const
		{
			value.deserialize(source);
		}
	};

	struct ByADL SZN_FINAL
	{
		template <class Value>
		void serialize(Sink &sink, const Value &value) const
		{
			using szn::serialize;
			serialize(sink, value);
		}

		template <class Value>
		void deserialize(Source &source, Value &value) const
		{
			using szn::deserialize;
			deserialize(source, value);
		}
	};
}


#endif
