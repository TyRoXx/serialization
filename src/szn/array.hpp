#ifndef SERIALIZATION_ARRAY_HPP_3AB95632_FCB4_481B_AB69_381567C9695C
#define SERIALIZATION_ARRAY_HPP_3AB95632_FCB4_481B_AB69_381567C9695C


#include <szn/util.hpp>
#include <boost/array.hpp>

#ifndef SZN_NO_CPP11
#	include <array>
#endif


namespace szn
{
	/// a format for fixed-length sequences of a certain type.
	/// Works with std::array and C arrays.
	template <std::size_t Length, class ElementFormat>
	struct array
	{
		typedef boost::array<typename ElementFormat::default_type, Length> default_type;

		template <class Sink, class Element>
		void serialize(Sink &sink, const boost::array<Element, Length> &a) const
		{
			BOOST_FOREACH (const Element &e, a)
			{
				szn::serialize(sink, e, ElementFormat());
			}
		}

		template <class Source, class Element>
		void deserialize(Source &source, boost::array<Element, Length> &a) const
		{
			for (std::size_t i = 0; i < Length; ++i)
			{
				szn::deserialize(source, a[i], ElementFormat());
			}
		}

#ifndef SZN_NO_CPP11
		template <class Sink, class Element>
		void serialize(Sink &sink, const std::array<Element, Length> &a) const
		{
			BOOST_FOREACH (const Element &e, a)
			{
				szn::serialize(sink, e, ElementFormat());
			}
		}

		template <class Source, class Element>
		void deserialize(Source &source, std::array<Element, Length> &a) const
		{
			for (std::size_t i = 0; i < Length; ++i)
			{
				szn::deserialize(source, a[i], ElementFormat());
			}
		}
#endif

		//C style array
		template <class Sink, class Element>
		void serialize(Sink &sink, const Element (&a)[Length]) const
		{
			BOOST_FOREACH (const Element &e, a)
			{
				szn::serialize(sink, e, ElementFormat());
			}
		}

		template <class Source, class Element>
		void deserialize(Source &source, Element (&a)[Length]) const
		{
			for (std::size_t i = 0; i < Length; ++i)
			{
				szn::deserialize(source, a[i], ElementFormat());
			}
		}
	};
}


#endif
