#ifndef SERIALIZATION_FLOAT_HPP_3A2C7AC8_6CC5_4BFF_BD0B_F1FB643EA623
#define SERIALIZATION_FLOAT_HPP_3A2C7AC8_6CC5_4BFF_BD0B_F1FB643EA623


#include "util.hpp"
#include <boost/integer.hpp>


namespace szn
{
	struct Sink;
	struct Source;


	template <class IntegerFormat>
	struct BinaryFloat
	{
		template <class Float>
		void serialize(Sink &sink, Float value) const
		{
			typename CompatibleInteger<Float>::type intValue;

			std::copy(reinterpret_cast<const char *>(&value),
					  reinterpret_cast<const char *>(&value) + sizeof(value),
					  reinterpret_cast<char *>(&intValue));

			using szn::serialize;
			serialize(sink, intValue, IntegerFormat());
		}

		template <class Float>
		void deserialize(Source &source, Float &value) const
		{
			typename CompatibleInteger<Float>::type intValue;

			using szn::deserialize;
			deserialize(source, intValue, IntegerFormat());

			std::copy(reinterpret_cast<const char *>(&intValue),
					  reinterpret_cast<const char *>(&intValue) + sizeof(intValue),
					  reinterpret_cast<char *>(&value));
		}

	private:

		template <class Float>
		struct CompatibleInteger
		{
			typedef typename boost::int_t<sizeof(Float) * 8>::exact type;
		};
	};


	template <std::size_t DecimalPrecision>
	struct ASCIIFloat
	{
		template <class Float>
		struct FloatPredicate : std::is_floating_point<Float> {};


		template <class Float>
		auto serialize(Sink &sink, Float value) const
			-> typename std::enable_if<FloatPredicate<Float>::value, void>::type
		{
			//TODO optimize
			std::ostringstream converter;
			converter.precision(DecimalPrecision);
			converter << value;

			const auto str = converter.str();
			sink.write(str.c_str(), str.size() + 1);
		}

		template <class Float>
		auto deserialize(Sink &sink, Float &value) const
			-> typename std::enable_if<FloatPredicate<Float>::value, void>::type
		{
			assert(nullptr == "TODO");
		}
	};
}


#endif
