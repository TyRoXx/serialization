#ifndef SERIALIZATION_BYTES_HPP_8BE0A18D_7A18_41A3_B154_C6C47903EDE7
#define SERIALIZATION_BYTES_HPP_8BE0A18D_7A18_41A3_B154_C6C47903EDE7


#include <szn/util.hpp>
#include <szn/format.hpp>
#include <szn/to_bytes.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/utility.hpp> //next
#include <boost/cstdint.hpp>
#include <string>
#include <vector>
#include <utility>
#include <stdexcept>
#include <cassert>


namespace szn
{
	/// a format for a variable-length sequence of bytes preceded by
	/// its length.
	/// Can be used with std::string
	template <class LengthFormat>
	struct bytes
	{
		static length_type const min_size = min_size<LengthFormat>::value;
		static length_type const max_size = ~static_cast<length_type>(0);

		typedef std::vector<char> default_type;

		template <class Sink, class FlexibleByteRange>
		void serialize(Sink &sink, FlexibleByteRange const &range) const
		{
			//For compilers without C++11 auto we have to use a method template
			//to get the result of to_bytes into a variable.
			//BOOST_AUTO does not work here for VC++ 2008.
			return serialize_impl(sink, to_bytes(range));
		}

		template <class Sink, class FlexibleByteRange>
		void serialize(Sink &sink, FlexibleByteRange const &range, length_type length) const
		{
			return serialize_impl(sink, to_bytes(range), length);
		}

	private:

		template <class Sink, class ByteRange>
		void serialize_impl(Sink &sink, ByteRange const &data) const
		{
			return serialize_impl(sink, data, boost::size(data));
		}

		template <class Sink, class ByteRange>
		void serialize_impl(Sink &sink, ByteRange const &data, length_type length) const
		{
			LengthFormat().serialize(sink, length);
			return serialize_range(sink, boost::begin(data), boost::end(data), length);
		}

	public:

		template <class Source>
		void deserialize(Source &source, std::string &str) const
		{
			return deserializeContainer(source, str);
		}

		template <class Source, class Byte, class Allocator>
		typename boost::enable_if_c<sizeof(Byte) == 1, void>::type
		deserialize(Source &source, std::vector<Byte, Allocator> &v) const
		{
			return deserializeContainer(source, v);
		}

		template <class Source, class ByteRandomAccessIterator>
		typename boost::enable_if_c<(boost::is_pointer<ByteRandomAccessIterator>::value) &&
									(sizeof(typename std::iterator_traits<ByteRandomAccessIterator>::value_type) == 1), void>::type
		deserialize(Source &source,
					std::pair<ByteRandomAccessIterator, ByteRandomAccessIterator> &range) const
		{
			if (!source.is_stable())
			{
				throw std::runtime_error(
					"Deserialization to a pair of iterators requires a stable source");
			}

			std::size_t length = 0;
			szn::deserialize(source, length, LengthFormat());
			source.load(length);
			range.first = reinterpret_cast<ByteRandomAccessIterator>(source.data());
			range.second = boost::next(range.first,
									   static_cast<typename std::iterator_traits<ByteRandomAccessIterator>::difference_type>(length));
		}

	private:

		template <class ByteIterator>
		struct is_char_pointer : boost::integral_constant<bool,
		    boost::is_pointer<ByteIterator>::value &&
		    (sizeof(typename std::iterator_traits<ByteIterator>::value_type) == 1)>
		{
		};

		template <class Sink, class ByteIterator>
		void serialize_range(Sink &sink, ByteIterator begin, ByteIterator end, length_type length) const
		{
			return serialize_range_impl(sink, begin, end, length, is_char_pointer<ByteIterator>());
		}

		template <class Sink, class ByteIterator>
		void serialize_range_impl(Sink &sink, ByteIterator begin, ByteIterator end, length_type length, boost::true_type) const
		{
			assert(length == static_cast<boost::uintmax_t>(std::distance(begin, end)));
			char const * const data = reinterpret_cast<char const *>(begin);
			sink.write(data, std::distance(begin, end));
		}

		template <class Sink, class ByteIterator>
		void serialize_range_impl(Sink &sink, ByteIterator begin, ByteIterator end, length_type length, boost::false_type) const
		{
			length_type n = 0;
			for (; (begin != end) && (n < length); ++begin, ++n)
			{
				char const c = *begin;
				sink.write(&c, 1);
			}
			if (n < length)
			{
				throw std::runtime_error("Input byte range too short");
			}
		}

		template <class Source, class Container>
		void deserializeContainer(Source &source, Container &destination) const
		{
			std::size_t length = 0;
			szn::deserialize(source, length, LengthFormat());
			source.load(length);
			if (source.size() < length)
			{
				assert(NULL == "TODO");
				return;
			}
			const char * const data = source.data();
			destination.assign(data, data + length);
			source.drop(length);
		}
	};
}


#endif
