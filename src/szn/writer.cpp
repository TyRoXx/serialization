#include "writer.hpp"
#include <szn/big_endian.hpp>
#include <szn/little_endian.hpp>
#include <szn/bool.hpp>


namespace szn
{
	writer::writer(sink &destination)
	    : m_destination(destination)
	{

	}

	sink &writer::destination() const
	{
		return m_destination;
	}

	writer &writer::be32(boost::uint32_t value)
	{
		szn::be32().serialize(m_destination, value);
		return *this;
	}

	writer &writer::boolean(bool value)
	{
		szn::boolean().serialize(m_destination, value);
		return *this;
	}

	writer &writer::raw(char const *data, size_t length)
	{
		m_destination.write(data, length);
		return *this;
	}
}
