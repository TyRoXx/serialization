#ifndef SERIALIZATION_WRITER_HPP
#define SERIALIZATION_WRITER_HPP


#include <szn/sink.hpp>
#include <szn/to_bytes.hpp>
#include <boost/cstdint.hpp>


namespace szn
{
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
			auto const bytes = to_bytes(value);
			LengthFormat().serialize(m_destination, bytes.second);
			return raw(bytes.first, bytes.second);
		}

	private:

		sink &m_destination;
	};
}


#endif
