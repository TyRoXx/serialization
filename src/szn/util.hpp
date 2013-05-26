#ifndef SERIALIZATION_UTIL_HPP_5595449D_4B56_4FD3_983C_CC91E7E873D2
#define SERIALIZATION_UTIL_HPP_5595449D_4B56_4FD3_983C_CC91E7E873D2


//GCC supports override and final beginning with version 4.7
#if defined(__GNUC__) && (__GNUC__ >= 4) && ((__GNUC__ > 4) || (__GNUC_MINOR__ >= 7))
#	define SZN_FINAL final
#	define SZN_OVERRIDE override

//Clang has supported override and final since 3.0 and this library requires Clang 3.2
#elif defined(__clang__)
#	define SZN_FINAL final
#	define SZN_OVERRIDE override

//VC++ 2010 is supported and it has override
#elif defined(_MSC_VER)
#	define SZN_FINAL
#	define SZN_OVERRIDE override

#else
#	define SZN_FINAL
#	define SZN_OVERRIDE
#endif


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
