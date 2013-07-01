#ifndef SERIALIZATION_TO_BYTES_HPP
#define SERIALIZATION_TO_BYTES_HPP


#include <cstring>
#include <vector>
#include <string>
#include <boost/range/iterator_range.hpp>


namespace szn
{
	template <class ByteRange>
	ByteRange const &to_bytes(ByteRange const &range)
	{
		return range;
	}

	template <class ByteRange>
	ByteRange const &&to_bytes(ByteRange const &&range)
	{
		return static_cast<ByteRange const &&>(range);
	}

	template <class C, class Traits, class Allocator>
	boost::iterator_range<char const *>
	to_bytes(std::basic_string<C, Traits, Allocator> const &str)
	{
		return boost::make_iterator_range(str.data(),
		                                  str.data() + str.size());
	}

	template <class C, class Allocator>
	typename std::enable_if<sizeof(C) == 1, boost::iterator_range<char const *>>::type
	to_bytes(std::vector<C, Allocator> const &vec)
	{
	    char const * const begin = reinterpret_cast<char const *>(vec.data());
		return boost::make_iterator_range(
					begin,
					begin + vec.size());
	}

	inline boost::iterator_range<char const *>
	to_bytes(char const *c_str)
	{
		return boost::make_iterator_range(c_str,
	                                      c_str + std::strlen(c_str));
	}

	template <class C, std::size_t N>
	typename std::enable_if<sizeof(C) == 1, boost::iterator_range<char const *>>::type
	to_bytes(C (&arr)[N])
	{
	    char const * const begin = reinterpret_cast<char const *>(&arr[0]);
		return boost::make_iterator_range(begin, begin + N);
	}
}


#endif
