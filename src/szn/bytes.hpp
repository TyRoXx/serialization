#ifndef SERIALIZATION_BYTES_HPP_8BE0A18D_7A18_41A3_B154_C6C47903EDE7
#define SERIALIZATION_BYTES_HPP_8BE0A18D_7A18_41A3_B154_C6C47903EDE7


#include <szn/util.hpp>
#include <boost/static_assert.hpp>


namespace szn
{
	/// a format for a variable-length sequence of bytes preceded by
	/// its length.
	/// Can be used with std::string
	template <class LengthFormat>
	struct Bytes
	{
		//std::string
		void serialize(Sink &sink, const std::string &str) const
		{
			szn::serialize(sink, str.length(), LengthFormat());
			sink.write(str.data(), str.length());
		}

		void deserialize(Source &source, std::string &str) const
		{
			return deserializeContainer(source, str);
		}

		//std::vector
		template <class Byte, class Allocator>
		typename std::enable_if<sizeof(Byte) == 1, void>::type
		serialize(Sink &sink, const std::vector<Byte, Allocator> &v) const
		{
			return serialize(sink, std::make_pair(v.begin(), v.end()));
		}

		template <class Byte, class Allocator>
		typename std::enable_if<sizeof(Byte) == 1, void>::type
		deserialize(Source &source, std::vector<Byte, Allocator> &v) const
		{
			return deserializeContainer(source, v);
		}

		//std::pair<Iterator, Iterator>
		template <class ByteRandomAccessIterator>
		typename std::enable_if<sizeof(typename std::iterator_traits<ByteRandomAccessIterator>::value_type) == 1, void>::type
		serialize(Sink &sink,
				  std::pair<ByteRandomAccessIterator, ByteRandomAccessIterator> range) const
		{
			using szn::serialize;
			const auto size = std::distance(range.first, range.second);
			serialize(sink, size, LengthFormat());
			const auto * const data = &*range.first;
			BOOST_STATIC_ASSERT(sizeof(*data) == 1);
			sink.write(reinterpret_cast<const char *>(data), size);
		}

		template <class ByteRandomAccessIterator>
		typename std::enable_if<(std::is_pointer<ByteRandomAccessIterator>::value) &&
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
			range.second = std::next(range.first,
									 static_cast<typename std::iterator_traits<ByteRandomAccessIterator>::difference_type>(length));
		}

	private:

		template <class Container>
		void deserializeContainer(Source &source, Container &destination) const
		{
			std::size_t length = 0;
			szn::deserialize(source, length, LengthFormat());
			source.load(length);
			if (source.size() < length)
			{
				assert(nullptr == "TODO");
				return;
			}
			const auto * const data = source.data();
			destination.assign(data, data + length);
			source.drop(length);
		}
	};
}


#endif