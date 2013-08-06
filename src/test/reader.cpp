#include <boost/test/unit_test.hpp>

#include <szn/reader.hpp>

namespace szn
{
	BOOST_AUTO_TEST_CASE(Serialization_reader)
	{
		unsigned char const data[] =
		{
			0xee,
			0xaa, 0xbb,
			0, 0, 0, 12,
			0, 0, 0, 0, 0, 0, 0, 1,

			0xee,
			0xbb, 0xaa,
			12, 0, 0, 0,
			1, 0, 0, 0, 0, 0, 0, 0,

			1,
			'r', 'a', 'w'
		};
		std::string const data_str(reinterpret_cast<char const *>(data), sizeof(data));
		BOOST_AUTO(source, make_range_source(data_str));

		boost::uint8_t a, e;
		boost::uint16_t b, f;
		boost::uint32_t c, g;
		boost::uint64_t d, h;
		bool i;
		char j[3];

		szn::reader(source)
			.be8(a)
			.be16(b)
			.be32(c)
			.be64(d)
			.le8(e)
			.le16(f)
			.le32(g)
			.le64(h)
			.boolean8(i)
			.raw(j, 3)
			;

		BOOST_CHECK_EQUAL(0xee, a);
		BOOST_CHECK_EQUAL(0xaabb, b);
		BOOST_CHECK_EQUAL(12, c);
		BOOST_CHECK_EQUAL(1, d);
		BOOST_CHECK_EQUAL(0xee, e);
		BOOST_CHECK_EQUAL(0xaabb, f);
		BOOST_CHECK_EQUAL(12, g);
		BOOST_CHECK_EQUAL(1, h);
		BOOST_CHECK_EQUAL(true, i);
		BOOST_CHECK_EQUAL("raw", std::string(j, 3));
	}
}
