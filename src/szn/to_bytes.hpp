#ifndef SERIALIZATION_TO_BYTES_HPP_E05554C3_20ED_47B5_ABDF_599CFF0763E9
#define SERIALIZATION_TO_BYTES_HPP_E05554C3_20ED_47B5_ABDF_599CFF0763E9


#include <cstring>
#include <vector>
#include <string>
#include <boost/range/iterator_range.hpp>


namespace szn
{
	template <class ByteRange>
	ByteRange &to_bytes(ByteRange &range)
	{
		return range;
	}

	namespace detail
	{
		//boost::enable_if_c<sizeof(C) == 1 ..> does not work for VC++ 2008,
		//but this workaround does.
		template <class C>
		struct is_char : boost::integral_constant<bool, sizeof(C) == 1>
		{
		};
	}

	template <class C, class Traits, class Allocator>
	typename boost::enable_if<detail::is_char<C>, boost::iterator_range<byte const *> >::type
	to_bytes(std::basic_string<C, Traits, Allocator> const &str)
	{
		byte const *begin = reinterpret_cast<byte const *>(str.data());
		return boost::make_iterator_range(begin, begin + str.size());
	}

	template <class C, class Allocator>
	typename boost::enable_if<detail::is_char<C>, boost::iterator_range<char const *>>::type
	to_bytes(std::vector<C, Allocator> const &vec)
	{
		char const * const begin = vec.empty() ? NULL : reinterpret_cast<char const *>(&vec.front());
		return boost::make_iterator_range(
					begin,
					begin + vec.size());
	}

	inline boost::iterator_range<byte const *>
	to_bytes(char const *c_str)
	{
	    byte const *begin = reinterpret_cast<byte const *>(c_str);
		return boost::make_iterator_range(begin,
	                                      begin + std::strlen(c_str));
	}

	template <class C, std::size_t N>
	typename boost::enable_if_c<sizeof(C) == 1, boost::iterator_range<byte const *> >::type
	to_bytes(C (&arr)[N])
	{
	    byte const * const begin = reinterpret_cast<byte const *>(&arr[0]);
		return boost::make_iterator_range(begin, begin + N);
	}
}


#endif
