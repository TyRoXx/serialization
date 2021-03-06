#ifndef SERIALIZATION_VECTOR_HPP_5418BAC1_2CDB_456D_A378_C673DE493A6D
#define SERIALIZATION_VECTOR_HPP_5418BAC1_2CDB_456D_A378_C673DE493A6D


#include <szn/util.hpp>
#include <szn/format.hpp>
#include <boost/foreach.hpp>
#include <boost/move/move.hpp>
#include <boost/array.hpp>
#include <boost/range/size.hpp>
#if SZN_HAS_ARRAY
#	include <array>
#endif


namespace szn
{
	/// a format for variable-length sequences of a certain type.
	/// Works with std::vector and std::map.
	template <class LengthFormat, class ElementFormat>
	struct vector
	{
		typedef std::vector<typename ElementFormat::default_type> default_type;

		static length_type const min_size = szn::min_size<LengthFormat>::value;
		//TODO: max_size can be limited by the length and/or element format
		static length_type const max_size = ~static_cast<length_type>(0);

		template <class Sink, class Container>
		void serialize(Sink &sink, Container const &v) const
		{
			serialize_impl(sink, v, typename std::iterator_traits<typename Container::const_iterator>::iterator_category());
		}

		template <class Sink, class First, class Second>
		void serialize(Sink &sink, std::pair<First, Second> const &elements) const
		{
			LengthFormat().serialize(sink, 2);
			ElementFormat const elem;
			elem.serialize(sink, elements.first);
			elem.serialize(sink, elements.second);
		}

		template <class Sink, class Element, std::size_t N>
		void serialize(Sink &sink, Element (&arr)[N]) const
		{
			LengthFormat().serialize(sink, N);
			serialize_raw_range(sink, arr);
		}

		template <class Sink, class InputIterator>
		void serialize(Sink &sink, InputIterator pos, std::size_t count) const
		{
			LengthFormat().serialize(sink, count);
			for (; count; ++pos, --count)
			{
				ElementFormat().serialize(sink, *pos);
			}
		}

		template <class Source, class SequenceContainer>
		void deserialize(Source &source, SequenceContainer &v) const
		{
			std::size_t length = 0;
			LengthFormat().deserialize(source, length);

			v.clear();
			reserve(v, length);

			for (std::size_t i = 0; i < length; ++i)
			{
				typedef typename make_map_key_mutable<typename SequenceContainer::value_type>::type element_type;
				element_type e;
				ElementFormat().deserialize(source, e);
				v.insert(v.end(), boost::move(e));
			}
		}

		template <class Source, class Element, std::size_t Length>
		void deserialize(Source &source, boost::array<Element, Length> &array) const
		{
			return deserialize_array<Element, Length>(source, array);
		}

#if SZN_HAS_ARRAY
		template <class Source, class Element, std::size_t Length>
		void deserialize(Source &source, std::array<Element, Length> &array) const
		{
			return deserialize_array<Element, Length>(source, array);
		}
#endif

	private:

		template <class Sink, class Range, class OtherTag>
		void serialize_impl(Sink &sink, Range const &sequence, OtherTag) const
		{
			LengthFormat().serialize(sink, sequence.size());
			serialize_raw_range(sink, sequence);
		}

		template <class Sink, class Range>
		void serialize_impl(Sink &sink, Range const &sequence, std::random_access_iterator_tag) const
		{
			LengthFormat().serialize(sink, boost::size(sequence));
			serialize_raw_range(sink, sequence);
		}

		template <class Sink, class Range>
		void serialize_raw_range(Sink &sink, Range const &sequence) const
		{
			for (auto i = boost::begin(sequence), e = boost::end(sequence); i != e; ++i)
			{
				szn::serialize(sink, *i, ElementFormat());
			}
		}

		template <class Element>
		struct make_map_key_mutable
		{
			typedef Element type;
		};

		template <class Key, class Value>
		struct make_map_key_mutable<std::pair<Key, Value>>
		{
			typedef std::pair<typename boost::remove_const<Key>::type, Value> type;
		};

		template <class Container>
		void reserve(Container &, size_t) const
		{
		}

		template <class Element, class Allocator>
		void reserve(std::vector<Element, Allocator> &v, size_t size) const
		{
			v.reserve(size);
		}

		template <class Element, std::size_t Length, class Source, class Array>
		void deserialize_array(Source &source, Array &array) const
		{
			std::size_t length = 0;
			LengthFormat().deserialize(source, length);
			if (length != Length)
			{
				//TODO more helpful error
				throw std::runtime_error("array length mismatch");
			}
			for (std::size_t i = 0; i < Length; ++i)
			{
				ElementFormat().deserialize(source, array[i]);
			}
		}
	};
}


#endif
