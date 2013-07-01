#ifndef SERIALIZATION_TO_BYTES_HPP
#define SERIALIZATION_TO_BYTES_HPP


#include <cstring>
#include <vector>


namespace szn
{
	inline std::pair<char const *, std::size_t>
	to_bytes(std::string const &str)
	{
		return std::make_pair(str.data(), str.size());
	}

	template <class C>
	typename std::enable_if<sizeof(C) == 1, std::pair<char const *, std::size_t>>::type
	to_bytes(std::vector<C> const &vec)
	{
		return std::make_pair(
					reinterpret_cast<char const *>(vec.data()),
					vec.size());
	}

	inline std::pair<char const *, std::size_t>
	to_bytes(char const *c_str)
	{
		return std::make_pair(c_str, std::strlen(c_str));
	}

	template <class C, std::size_t N>
	typename std::enable_if<sizeof(C) == 1, std::pair<char const *, std::size_t>>::type
	to_bytes(C (&arr)[N])
	{
		return std::make_pair(reinterpret_cast<char const *>(&arr[0]), N);
	}

	template <class C>
	typename std::enable_if<sizeof(C) == 1, std::pair<char const *, std::size_t>>::type
	to_bytes(std::pair<C const *, C const *> const &pair)
	{
		auto length = std::distance(pair.first, pair.second);
		return std::make_pair(pair.first, length);
	}
}


#endif
