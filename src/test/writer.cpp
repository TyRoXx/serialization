#include <boost/test/unit_test.hpp>

#include <szn/writer.hpp>

namespace szn
{
	BOOST_AUTO_TEST_CASE(Serialization_dynamic_message)
	{
		std::string generated;
		BOOST_AUTO(sink, make_container_sink(generated));

		szn::writer(sink)
			.be8(0xee)
			.be16(0xaabb)
			.be32(12)
			.be64(1)
			.le8(0xee)
			.le16(0xaabb)
			.le32(12)
			.le64(1)
			.bytes<be16>("hello", 5)
			.boolean8(true)
			.raw("raw", 3)
			;

		byte const expectedBytes[] =
		{
		    0xee,
		    0xaa, 0xbb,
		    0, 0, 0, 12,
		    0, 0, 0, 0, 0, 0, 0, 1,

		    0xee,
		    0xbb, 0xaa,
		    12, 0, 0, 0,
		    1, 0, 0, 0, 0, 0, 0, 0,

		    0, 5, 'h', 'e', 'l', 'l', 'o',
		    1,
		    'r', 'a', 'w'
		};
		BOOST_CHECK_EQUAL(generated,
		                  std::string(reinterpret_cast<char const *>(&expectedBytes), sizeof(expectedBytes)));
	}
}
