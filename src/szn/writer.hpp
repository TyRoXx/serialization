#ifndef SERIALIZATION_WRITER_HPP
#define SERIALIZATION_WRITER_HPP


#include <szn/sink.hpp>
#include <boost/cstdint.hpp>
#include <boost/range/size.hpp>
#include <cstring>
#include <vector>


namespace szn
{
	namespace detail
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
	}

	struct writer SZN_FINAL
	{
		explicit writer(sink &destination);
		sink &destination() const;
		writer &be32(boost::uint32_t value);
		writer &boolean(bool value);
		writer &raw(char const *data, size_t length);

		template <class LengthFormat, class ByteContainer>
		writer &bytes(ByteContainer const &value)
		{
			using detail::to_bytes;
			auto const bytes = to_bytes(value);
			LengthFormat().serialize(m_destination, bytes.second);
			return raw(bytes.first, bytes.second);
		}

	private:

		sink &m_destination;
	};
}


#endif
