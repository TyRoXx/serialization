#ifndef SERIALIZATION_VECTOR_HPP_5418BAC1_2CDB_456D_A378_C673DE493A6D
#define SERIALIZATION_VECTOR_HPP_5418BAC1_2CDB_456D_A378_C673DE493A6D


#include <szn/util.hpp>
#include <boost/foreach.hpp>
#include <boost/move/move.hpp>


namespace szn
{
	/// a format for variable-length sequences of a certain type.
	/// Works with std::vector.
	template <class LengthFormat, class ElementFormat>
	struct vector
	{
		typedef std::vector<typename ElementFormat::default_type> default_type;

		template <class Sink, class SizedSequence>
		void serialize(Sink &sink, SizedSequence const &v) const
		{
			szn::serialize(sink, v.size(), LengthFormat());

			BOOST_FOREACH (const typename SizedSequence::value_type &e, v)
			{
				szn::serialize(sink, e, ElementFormat());
			}
		}

		template <class Source, class Sequence>
		void deserialize(Source &source, Sequence &v) const
		{
			std::size_t length = 0;
			szn::deserialize(source, length, LengthFormat());

			v.clear();
			v.reserve(length);

			for (std::size_t i = 0; i < length; ++i)
			{
				typename Sequence::value_type e;
				szn::deserialize(source, e, ElementFormat());
				v.push_back(boost::move(e));
			}
		}
	};
}


#endif
