#ifndef SERIALIZATION_UTIL_HPP_5595449D_4B56_4FD3_983C_CC91E7E873D2
#define SERIALIZATION_UTIL_HPP_5595449D_4B56_4FD3_983C_CC91E7E873D2


#ifndef SZN_NO_CPP11

#if defined(__GNUC__) && (__GNUC__ >= 4) && (__GNUC_MINOR__ >= 6)
//	GCC supports override and final beginning with version 4.7
#	if ((__GNUC__ > 4) || (__GNUC_MINOR__ >= 7))
#		define SZN_FINAL final
#		define SZN_OVERRIDE override
#		define SZN_HAS_UNIQUE_PTR 1
#	else
#		if (__GNUC_MINOR__ >= 4)
#			define SZN_HAS_UNIQUE_PTR 1
#		else
#			define SZN_HAS_UNIQUE_PTR 0
#		endif
#		define SZN_FINAL
#		define SZN_OVERRIDE
#	endif
//	GCC 4.6 introduced noexcept
#	define SZN_NOEXCEPT noexcept
#	define SZN_HAS_ENUM_CLASS 1

#elif defined(__clang__)
//	Clang has supported override, final and noexcept since 3.0
#	define SZN_FINAL final
#	define SZN_OVERRIDE override
#	define SZN_NOEXCEPT noexcept
#	define SZN_HAS_ENUM_CLASS 1
#	define SZN_HAS_UNIQUE_PTR 1

#elif defined(_MSC_VER)
//	VC++ 2010 has override
#	define SZN_FINAL
#	define SZN_OVERRIDE override
#	define SZN_NOEXCEPT
#	define SZN_HAS_ENUM_CLASS 0
#	define SZN_HAS_UNIQUE_PTR 1

#endif

#endif //#ifndef SZN_NO_CPP11

#ifndef SZN_FINAL //unknown compiler or SZN_NO_CPP11 defined
#	define SZN_FINAL
#	define SZN_OVERRIDE
#	define SZN_NOEXCEPT
#	define SZN_HAS_ENUM_CLASS 0
#	define SZN_HAS_UNIQUE_PTR 0
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
