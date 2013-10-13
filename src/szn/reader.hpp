#ifndef SERIALIZATION_READER_HPP_
#define SERIALIZATION_READER_HPP_


#include <szn/source.hpp>
#include <szn/big_endian.hpp>
#include <szn/little_endian.hpp>
#include <szn/bool.hpp>
#include <szn/bytes.hpp>
#include <boost/cstdint.hpp>


namespace szn
{
	struct reader SZN_FINAL
	{
		explicit reader(source &from);
		source &from() const;
		reader &be8(boost::uint8_t &value);
		reader &be16(boost::uint16_t &value);
		reader &be32(boost::uint32_t &value);
		reader &be64(boost::uint64_t &value);
		reader &le8(boost::uint8_t &value);
		reader &le16(boost::uint16_t &value);
		reader &le32(boost::uint32_t &value);
		reader &le64(boost::uint64_t &value);
		reader &boolean8(bool &value);
		reader &raw(char *data, size_t length);

		template <class Format, class Value>
		reader &value(Value &value)
		{
			Format().deserialize(m_from, value);
			return *this;
		}

	private:

		source &m_from;
	};


	inline reader::reader(source &from)
		: m_from(from)
	{
	}

	inline source &reader::from() const
	{
		return m_from;
	}

	inline reader &reader::be8(boost::uint8_t &value)
	{
		return this->value<szn::be8>(value);
	}

	inline reader &reader::be16(boost::uint16_t &value)
	{
		return this->value<szn::be16>(value);
	}

	inline reader &reader::be32(boost::uint32_t &value)
	{
		return this->value<szn::be32>(value);
	}

	inline reader &reader::be64(boost::uint64_t &value)
	{
		return this->value<szn::be64>(value);
	}

	inline reader &reader::le8(boost::uint8_t &value)
	{
		return this->value<szn::le8>(value);
	}

	inline reader &reader::le16(boost::uint16_t &value)
	{
		return this->value<szn::le16>(value);
	}

	inline reader &reader::le32(boost::uint32_t &value)
	{
		return this->value<szn::le32>(value);
	}

	inline reader &reader::le64(boost::uint64_t &value)
	{
		return this->value<szn::le64>(value);
	}

	inline reader &reader::boolean8(bool &value)
	{
		return this->value<szn::boolean>(value);
	}

	inline reader &reader::raw(char *data, size_t length)
	{
		m_from.read(data, length);
		return *this;
	}
}


#endif
