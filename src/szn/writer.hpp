#ifndef SERIALIZATION_WRITER_HPP
#define SERIALIZATION_WRITER_HPP


#include <szn/sink.hpp>
#include <szn/big_endian.hpp>
#include <szn/little_endian.hpp>
#include <szn/bytes.hpp>
#include <szn/bool.hpp>
#include <boost/cstdint.hpp>


namespace szn
{
	struct writer SZN_FINAL
	{
		explicit writer(sink &destination);
		sink &destination() const;
		writer &be8(boost::uint8_t value);
		writer &be16(boost::uint16_t value);
		writer &be32(boost::uint32_t value);
		writer &be64(boost::uint64_t value);
		writer &le8(boost::uint8_t value);
		writer &le16(boost::uint16_t value);
		writer &le32(boost::uint32_t value);
		writer &le64(boost::uint64_t value);
		writer &boolean8(bool value);
		writer &raw(char const *data, size_t length);

		template <class LengthFormat, class ByteContainer>
		writer &bytes(ByteContainer const &value)
		{
			return this->value<szn::bytes<LengthFormat>>(value);
		}

		template <class Format, class Value>
		writer &value(Value const &value)
		{
			Format().serialize(m_destination, value);
			return *this;
		}

	private:

		sink &m_destination;
	};


	writer::writer(sink &destination)
	    : m_destination(destination)
	{
	}

	sink &writer::destination() const
	{
		return m_destination;
	}

	writer &writer::be8(boost::uint8_t value)
	{
		return this->value<szn::be8>(value);
	}

	writer &writer::be16(boost::uint16_t value)
	{
		return this->value<szn::be16>(value);
	}

	writer &writer::be32(boost::uint32_t value)
	{
		return this->value<szn::be32>(value);
	}

	writer &writer::be64(boost::uint64_t value)
	{
		return this->value<szn::be64>(value);
	}

	writer &writer::le8(boost::uint8_t value)
	{
		return this->value<szn::le8>(value);
	}

	writer &writer::le16(boost::uint16_t value)
	{
		return this->value<szn::le16>(value);
	}

	writer &writer::le32(boost::uint32_t value)
	{
		return this->value<szn::le32>(value);
	}

	writer &writer::le64(boost::uint64_t value)
	{
		return this->value<szn::le64>(value);
	}

	writer &writer::boolean8(bool value)
	{
		return this->value<szn::boolean>(value);
	}

	writer &writer::raw(char const *data, size_t length)
	{
		m_destination.write(data, length);
		return *this;
	}
}


#endif
