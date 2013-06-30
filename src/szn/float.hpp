#ifndef SERIALIZATION_FLOAT_HPP_3A2C7AC8_6CC5_4BFF_BD0B_F1FB643EA623
#define SERIALIZATION_FLOAT_HPP_3A2C7AC8_6CC5_4BFF_BD0B_F1FB643EA623


#include <szn/util.hpp>
#include <boost/integer.hpp>
#include <boost/type_traits/is_floating_point.hpp>


namespace szn
{
	struct sink;
	struct source;


	template <class IntegerFormat>
	struct binary_float
	{
		template <class Float>
		void serialize(sink &sink, Float value) const
		{
			typename compatible_int<Float>::type intValue;

			std::copy(reinterpret_cast<const char *>(&value),
					  reinterpret_cast<const char *>(&value) + sizeof(value),
					  reinterpret_cast<char *>(&intValue));

			using szn::serialize;
			serialize(sink, intValue, IntegerFormat());
		}

		template <class Float>
		void deserialize(source &source, Float &value) const
		{
			typename compatible_int<Float>::type intValue;

			using szn::deserialize;
			deserialize(source, intValue, IntegerFormat());

			std::copy(reinterpret_cast<const char *>(&intValue),
					  reinterpret_cast<const char *>(&intValue) + sizeof(intValue),
					  reinterpret_cast<char *>(&value));
		}

	private:

		template <class Float>
		struct compatible_int
		{
			typedef typename boost::int_t<sizeof(Float) * 8>::exact type;
		};
	};


	template <std::size_t DecimalPrecision>
	struct ascii_float
	{
		template <class Float>
		struct float_predicate : boost::is_floating_point<Float> {};


		template <class Float>
		typename boost::enable_if_c<float_predicate<Float>::value, void>::type
		serialize(sink &sink, Float value) const
		{
			//TODO optimize
			std::ostringstream converter;
			converter.precision(DecimalPrecision);
			converter << value;

			const std::string str = converter.str();
			sink.write(str.c_str(), str.size() + 1);
		}

		template <class Float>
		typename boost::enable_if_c<float_predicate<Float>::value, void>::type
		deserialize(sink &sink, Float &value) const
		{
			assert(NULL == "TODO");
		}
	};
}


#endif
