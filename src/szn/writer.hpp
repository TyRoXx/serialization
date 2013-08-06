#ifndef SERIALIZATION_WRITER_HPP_7DD288A6_4622_4506_841C_64CDFD748CFE
#define SERIALIZATION_WRITER_HPP_7DD288A6_4622_4506_841C_64CDFD748CFE


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

		template <class LengthFormat, class FlexibleByteRange>
		writer &bytes(FlexibleByteRange const &value)
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


	inline writer::writer(sink &destination)
	    : m_destination(destination)
	{
	}

	inline sink &writer::destination() const
	{
		return m_destination;
	}

	inline writer &writer::be8(boost::uint8_t value)
	{
		return this->value<szn::be8>(value);
	}

	inline writer &writer::be16(boost::uint16_t value)
	{
		return this->value<szn::be16>(value);
	}

	inline writer &writer::be32(boost::uint32_t value)
	{
		return this->value<szn::be32>(value);
	}

	inline writer &writer::be64(boost::uint64_t value)
	{
		return this->value<szn::be64>(value);
	}

	inline writer &writer::le8(boost::uint8_t value)
	{
		return this->value<szn::le8>(value);
	}

	inline writer &writer::le16(boost::uint16_t value)
	{
		return this->value<szn::le16>(value);
	}

	inline writer &writer::le32(boost::uint32_t value)
	{
		return this->value<szn::le32>(value);
	}

	inline writer &writer::le64(boost::uint64_t value)
	{
		return this->value<szn::le64>(value);
	}

	inline writer &writer::boolean8(bool value)
	{
		return this->value<szn::boolean>(value);
	}

	inline writer &writer::raw(char const *data, size_t length)
	{
		m_destination.write(data, length);
		return *this;
	}
}


#endif
