#ifndef SERIALIZATION_VECTOR_H
#define SERIALIZATION_VECTOR_H


#include "util.h"
#include <boost/foreach.hpp>


namespace szn
{
	/// a format for variable-length sequences of a certain type.
	/// Works with std::vector.
	template <class LengthFormat, class ElementFormat>
	struct Vector
	{
		template <class Element>
		void serialize(Sink &sink, const std::vector<Element> &v) const
		{
			szn::serialize(sink, v.size(), LengthFormat());

			BOOST_FOREACH (const auto &e, v)
			{
				szn::serialize(sink, e, ElementFormat());
			}
		}

		template <class Element>
		void deserialize(Source &source, std::vector<Element> &v) const
		{
			std::size_t length = 0;
			szn::deserialize(source, length, LengthFormat());

			v.clear();
			v.reserve(length);

			for (std::size_t i = 0; i < length; ++i)
			{
				Element e;
				szn::deserialize(source, e, ElementFormat());
				v.push_back(std::move(e));
			}
		}
	};
}


#endif
