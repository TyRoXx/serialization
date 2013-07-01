#ifndef SERIALIZATION_BYTES_HPP_8BE0A18D_7A18_41A3_B154_C6C47903EDE7
#define SERIALIZATION_BYTES_HPP_8BE0A18D_7A18_41A3_B154_C6C47903EDE7


#include <szn/util.hpp>
#include <szn/format.hpp>
#include <szn/to_bytes.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/utility.hpp> //next
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
		static std::size_t const min_size = min_size<LengthFormat>::value;
		static std::size_t const max_size = ~static_cast<std::size_t>(0);

		typedef std::vector<char> default_type;

		template <class Sink, class ByteRange>
		void serialize(Sink &sink, const ByteRange &range) const
		{
			auto const data = to_bytes(range);
			LengthFormat().serialize(sink, data.second);
			sink.write(data.first, data.second);
		}

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
			if (!source.isStable())
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
